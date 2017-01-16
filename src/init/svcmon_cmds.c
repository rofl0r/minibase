#include <sys/kill.h>
#include <sys/write.h>

#include <format.h>
#include <util.h>

#include "svcmon.h"

#define PONLY 0
#define GROUP 1

static void killrec(struct svcrec* rc, int group, int sig)
{
	int pid = rc->pid;

	if(group) pid = -pid;

	syskill(pid, sig);
}

static void dumpstate(void)
{
	int blen = PAGE;
	char* buf = alloc(blen);

	if(!buf) return;

	char* p = buf;
	char* e = buf + blen;
	struct svcrec* rc;

	for(rc = firstrec(); rc; rc = nextrec(rc)) {
		p = fmtstr(p, e, " ");
		if(rc->pid > 0)
			p = fmtpad(p, e, 5, fmtint(p, e, rc->pid));
		else
			p = fmtpad(p, e, 5, fmtstr(p, e, "-"));
		p = fmtstr(p, e, rc->tail != rc->head ? "*" : " ");
		p = fmtstr(p, e, " ");
		p = fmtstr(p, e, rc->name);
		p = fmtstr(p, e, "\n");
	}

	writeall(gg.outfd, buf, p - buf);

	afree();
}

static void dumpstatus(struct svcrec* rc)
{
	if(!rc->ring)
		return;

	char* ring = rc->ring;
	int head = rc->head;
	int tail = rc->tail;

	writeall(gg.outfd, "#", 1);

	if(tail > head) {
		writeall(gg.outfd, ring + head, tail - head);
	} else {
		writeall(gg.outfd, ring + head, RINGSIZE - head);
		writeall(gg.outfd, ring, tail);
	}
}

static void dumpidof(struct svcrec* rc)
{
	char buf[40];

	char* p = buf;
	char* e = buf + sizeof(buf) - 1;

	if(rc->pid <= 0)
		return;

	p = fmtstr(p, e, "#");
	p = fmtint(p, e, rc->pid);
	*p++ = '\n';

	syswrite(gg.outfd, buf, p - buf);
}

static void disable(struct svcrec* rc)
{
	rc->lastsig = 0;
	rc->flags |= P_DISABLED;
	gg.state |= S_PASSREQ;
}

static void enable(struct svcrec* rc)
{
	rc->lastrun = 0;
	rc->flags &= ~P_DISABLED;
	gg.state |= S_PASSREQ;
}

void reboot(char code)
{
	gg.rbcode = code;
	stopall();
}

void parsecmd(char* cmd)
{
	char* arg = cmd + 1;
	struct svcrec* rc = NULL;

	/* Check whether this command needs arguments */
	switch(*cmd) {
		/* Mandatory argument */
		case 'q':
		case 'e': case 'd':	/* enable, disable */
		case 'p': case 'w':	/* pause, resume */
		case 'h': case 'i':	/* hup, pidof */
			if(!(rc = findrec(arg)))
				return report("no entry named", arg, 0);
			break;

		/* There are no commands with optional arguments atm */
		/* There are few that take no argument at all however */
		default: if(*arg)
			return report("no argument allowed for", cmd, 0);
	}

	/* Now the command itself */
	switch(*cmd) {
		/* halt */
		case 'H': reboot('h'); break;
		case 'P': reboot('p'); break;
		case 'R': reboot('r'); break;
		/* process ops */
		case 'p': killrec(rc, GROUP, SIGSTOP); break;
		case 'w': killrec(rc, GROUP, SIGCONT); break;
		case 'h': killrec(rc, PONLY, SIGHUP); break;
		case 'd': disable(rc); break;
		case 'e': enable(rc); break;
		/* state query */
		case 'i': dumpidof(rc); break;
		case 'q': dumpstatus(rc); break;
		case '?': dumpstate(); break;
		/* reconfigure */
		case 'c': reload(); break;
		default: report("unknown command", cmd, 0);
	}
}
