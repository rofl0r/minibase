#include <sys/open.h>
#include <sys/close.h>
#include <sys/mmap.h>
#include <sys/munmap.h>

#include <sys/setresuid.h>
#include <sys/setresgid.h>
#include <sys/setgroups.h>

#include <string.h>
#include <format.h>

#include "msh.h"
#include "msh_cmd.h"

static int mapid(struct mbuf* mb, char* name)
{
	char* filedata = mb->buf;
	char* fileend = filedata + mb->len;
	int id;

	/* user:x:500:...\n */
	/* ls  ue un     le */
	char *ls, *le;
	char *ue, *un;
	char *ne = NULL;
	for(ls = filedata; ls < fileend; ls = le + 1) {
		le = strecbrk(ls, fileend, '\n');
		ue = strecbrk(ls, le, ':');
		if(ue >= le) continue;
		un = strecbrk(ue + 1, le, ':') + 1;
		if(un >= le) continue;

		if(strncmp(name, ls, ue - ls))
			continue;

		ne = parseint(un, &id);
		break;
	};

	if(!ne || *ne != ':')
		return -1;

	return id;
}

static int pwname2id(struct mbuf* mb, char* name)
{
	int id;
	char* p;

	if((p = parseint(name, &id)) && !*p)
		return id;

	return mapid(mb, name);
}

static int pwresolve(struct sh* ctx, char* pwfile,
                     int n, char** names, int* ids, char* notfound)
{
	struct mbuf mb;
	int err = 0, ret, i;

	if((ret = mmapfile(&mb, pwfile)) < 0)
		return error(ctx, "cannot mmap", pwfile, ret);

	for(i = 0; i < n; i++)
		if((ids[i] = pwname2id(&mb, names[i])) < 0)
			err |= error(ctx, notfound, names[i], 0);

	munmapfile(&mb);

	return err;
}

int cmd_setuid(struct sh* ctx, int argc, char** argv)
{
	int ret, uid;
	char* pwfile = "/etc/passwd";

	if((ret = numargs(ctx, argc, 2, 2)))
		return ret;
	if((ret = pwresolve(ctx, pwfile, 1, &argv[1], &uid, "unknown user")))
		return ret;

	return fchk(sys_setresuid(uid, uid, uid), ctx, "setuid", argv[1]);
}

int cmd_setgid(struct sh* ctx, int argc, char** argv)
{
	int ret, gid;
	char* pwfile = "/etc/group";

	if((ret = numargs(ctx, argc, 2, 2)))
		return ret;
	if((ret = pwresolve(ctx, pwfile, 1, &argv[1], &gid, "unknown group")))
		return ret;

	return fchk(sys_setresgid(gid, gid, gid), ctx, "setgid", argv[1]);
}

int cmd_groups(struct sh* ctx, int argc, char** argv)
{
	char* pwfile = "/etc/group";
	int ng = argc - 1;
	int grp[ng];
	int ret;

	if((ret = numargs(ctx, argc, 2, 0)))
		return ret;
	if((ret = pwresolve(ctx, pwfile, ng, &argv[1], grp, "unknown group")))
		return ret;

	return fchk(sys_setgroups(ng, grp), ctx, "setgroups", NULL);
}