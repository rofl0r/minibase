#include <bits/fcntl.h>
#include <sys/sched.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/mman.h>

#include <errtag.h>
#include <string.h>
#include <alloca.h>
#include <util.h>

/* This tiny executable gets spawned as PID 1, either by the kernel
   or by switchroot if the system boots with initrd. Its task is
   to run sequential initialization and proceed to reboot if it fails.

   If everything goes well, it execs into the supervisor.

   Unlike the traditional /sbin/init, this one is a short-lived process
   that does not do any supervision itself. */

#define PAGE 4096

ERRTAG("init");
ERRLIST(NENOENT NEACCES NEINVAL NEBADF NESRCH NEFAULT NEPERM NEMFILE
	NENFILE NEISDIR NENOTDIR NELOOP);

static void set_std_fds(void)
{
	int fd;

	if(sys_fcntl(2, F_GETFD) >= 0)
		return; /* if 2 is ok, then 0 and 1 must be valid as well */

	if((fd = sys_open("/dev/null", O_RDWR)) >= 0)
		goto gotfd;
	if((fd = sys_open("/", O_RDONLY)) >= 0)
		goto gotfd;

	/* Not being able to open / read-only is weird enough to panic */
	/* Oh yeah and we can't fail() here, fail() writes to stderr.  */
	_exit(0x99);

gotfd:
	if(fd < 1)
		sys_dup2(fd, 1);
	if(fd < 2)
		sys_dup2(fd, 2);
	if(fd > 2)
		sys_close(fd);
}

static int count_env(char** envp)
{
	char** q;

	for(q = envp; *q; q++)
		;

	return q - envp;
}

static void set_new_env(char** newenv, int newcnt, char** envp)
{
	char** p = newenv;
	char** e = newenv + newcnt - 1;

	*p++ = "PATH=/bin:/sbin";

	char** q;

	for(q = envp; *q && p < e; q++)
		*p++ = *q;

	*p++ = NULL;
}

static int open_stat(const char* name, int* len)
{
	long fd = sys_open(name, O_RDONLY);

	if(fd < 0 && fd != -ENOENT)
		warn("open", name, fd);
	if(fd < 0)
		return -1;

	struct stat st;
	long ret;

	if((ret = sys_fstat(fd, &st)) < 0) {
		warn("stat", name, ret);
		return -1;
	}

	if(st.size > 0x10000) {
		warn("file too large:", name, 0);
		return -1;
	}

	*len = st.size;
	return fd;
}

static int read_whole(int fd, const char* name, char* buf, int len)
{
	long rd;
	char* ptr = buf;

	while(len > 0) {
		rd = sys_read(fd, ptr, len);

		if(rd < 0) {
			warn("read", name, rd);
			return -1;
		} else if(rd == 0) {
			break;
		}

		ptr += rd;
		len -= rd;
	}

	return ptr - buf;
}

int count_lines(char* buf, int len)
{
	char* end = buf + len;
	char* p;
	int count = 0;

	for(p = buf; p < end; p++)
		if(*p == '\n')
			count++;

	return count + 1; /* overestimate a bit */
}

static int round_to_page(int len)
{
	return len + (PAGE - len % PAGE) % PAGE;
}

static int extend(char* brk, int curr, int len)
{
	int alloc = round_to_page(len);

	if(len <= curr)
		return curr;

	char* end = sys_brk(brk + alloc);

	if(brk_error(brk, end)) {
		warn("out of memory", NULL, 0);
		return -1;
	}

	return alloc;
}

static void set_env_pointers(char** envp, int envc, char* buf, int len)
{
	char* end = buf + len;
	char* p;
	int nl = 1;
	int i = 0;

	for(p = buf; p < end; p++) {
		if(*p == '\n') {
			nl = 1;
		} else if(nl && *p == '#') {
			nl = 0;
		} else if(nl && i < envc) {
			envp[i++] = p;
			nl = 0;
		}
	}

	envp[i++] = NULL;
}

static char** load_env_file(const char* name)
{
	int alloc;
	int len;
	int fd;

	if((fd = open_stat(name, &len)) < 0)
		return NULL;

	char* brk = sys_brk(NULL);

	/* rough guess on avg envline size */
	if((alloc = extend(brk, 0, len + sizeof(char*)*len/10)) < 0)
		return NULL;
	if((len = read_whole(fd, name, brk, len)) < 0)
		return NULL;

	int skip = len + 1;
	int envc = count_lines(brk, len) + 1;
	char** envp = (char**)(brk + skip);

	if((alloc = extend(brk, alloc, skip + envc*sizeof(char*))) < 0)
		return NULL;

	set_env_pointers(envp, envc, brk, len);

	return envp;
}

static int exec_one(char* cmd, char** envp)
{
	char* args[] = { cmd, NULL };
	return sys_execve(cmd, args, envp);
}

static void exec_into(char* cmd, char** envp)
{
	long ret = exec_one(cmd, envp);
	warn("exec", cmd, ret);
}

static void spawn_if_exists(char* cmd, char** envp)
{
	int pid = sys_fork();

	if(pid < 0)
		warn("fork", NULL, pid);

	if(pid == 0) {
		long ret = exec_one(cmd, envp);
		if(ret != -ENOENT)
			fail("exec", cmd, ret);
		_exit(0x0);
	} else {
		int status;
		sys_waitpid(pid, &status, 0);
	}
}

static void sleep(int sec)
{
	struct timespec ts = { sec, 0 };

	sys_nanosleep(&ts, NULL);
}

int main(int argc, char** argv, char** envp)
{
	char** newenv;

	set_std_fds();

	if(!(newenv = load_env_file("/etc/sysenv"))) {
		int envc = count_env(envp);
		int ncnt = envc + 10;
		newenv = alloca(ncnt*sizeof(char*));
		set_new_env(newenv, ncnt, envp);
	}

	spawn_if_exists("/etc/sysinit", newenv);
	exec_into("/sbin/system/super", newenv);

	warn("startup failed, rebooting in 5 seconds", NULL, 0);
	sleep(5);
	exec_into("/sbin/system/reboot", newenv);

	return -1;
}