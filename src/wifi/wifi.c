#include <bits/socket/unix.h>
#include <bits/errno.h>

#include <sys/file.h>
#include <sys/socket.h>
#include <sys/mman.h>

#include <nlusctl.h>
#include <format.h>
#include <string.h>
#include <util.h>
#include <main.h>

#include "common.h"
#include "wifi.h"

ERRTAG("wifi");
ERRLIST(NENOENT NEINVAL NENOSYS NENOENT NEACCES NEPERM NEBUSY NEALREADY
	NENETDOWN NENOKEY NENOTCONN NENODEV NETIMEDOUT NENOBUFS NEISCONN
	NEAGAIN);

void* heap_alloc(CTX, uint size)
{
	void* brk = ctx->brk;
	void* ptr = ctx->ptr;

	if(!brk) {
		brk = sys_brk(NULL);
		ptr = brk;
	}

	ulong left = brk - ptr;

	if(left < size) {
		ulong need = pagealign(size - left);
		void* new = sys_brk(brk + need);

		if(brk_error(brk, new))
			fail("cannot allocate memory", NULL, 0);

		brk = new;
	}

	ctx->ptr = ptr + size;
	ctx->brk = brk;

	return ptr;
}

static void connect_to_wictl(CTX)
{
	int ret, fd;
	struct sockaddr_un addr = {
		.family = AF_UNIX,
		.path = WICTL
	};

	if((fd = sys_socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		fail("socket", "AF_UNIX", fd);
	if((ret = sys_connect(fd, &addr, sizeof(addr))) < 0)
		fail("connect", addr.path, ret);

	ctx->fd = fd;
	ctx->connected = 1;

	uc_buf_set(&ctx->uc, ctx->txbuf, sizeof(ctx->txbuf));
}

static void send_command(CTX)
{
	int wr, fd = ctx->fd;

	if(!ctx->connected)
		fail("socket not connected", NULL, 0);

	if((wr = uc_send_whole(fd, &ctx->uc)) < 0)
		fail("uc-send", NULL, wr);
}

static struct ucmsg* recv_reply(CTX)
{
	struct urbuf* ur = &ctx->ur;
	int ret, fd = ctx->fd;

	if((ret = uc_recv_shift(fd, ur)) < 0)
		fail("recv", "wictl", ret);

	return ur->msg;
}

/* We cannot use small ctx->rxbuf to receive AP list, which tends to be
   several KB in size in most cases. So we need to switch ctx->ur from
   txbuf to a large heap-allocated buffer, and optionally grow the buffer
   if the messages happens to be larger the initial estimate of 1 page. */

static struct ucmsg* recv_large(CTX)
{
	struct urbuf* ur = &ctx->ur;
	int ret, fd = ctx->fd;
	int len = 4096;

	if(ur->buf == ctx->rxbuf) {
		void* buf = heap_alloc(ctx, len);

		ur_buf_change(ur, buf, len);
	}

	while((ret = uc_recv_shift(fd, ur)) < 0) {
		if(ret != -ENOBUFS)
			fail("recv", "wictl", ret);

		(void)heap_alloc(ctx, len);

		ur->end += len;
	}

	ctx->ptr = ur->rptr;

	return ur->msg;
}

struct ucmsg* send_recv_msg(CTX)
{
	send_command(ctx);

	return recv_reply(ctx);
}

struct ucmsg* send_recv_aps(CTX)
{
	send_command(ctx);

	return recv_large(ctx);
}

int send_recv_cmd(CTX)
{
	struct ucmsg* msg = send_recv_msg(ctx);

	return msg->cmd;
}

/* Command line args stuff */

static void init_context(CTX, int argc, char** argv)
{
	ctx->argi = 1;
	ctx->argc = argc;
	ctx->argv = argv;

	ur_buf_set(&ctx->ur, ctx->rxbuf, sizeof(ctx->rxbuf));
}

static void no_other_options(CTX)
{
	if(ctx->argi < ctx->argc)
		fail("too many arguments", NULL, 0);
	if(ctx->opts)
		fail("bad options", NULL, 0);
}

static char* shift_arg(CTX)
{
	if(ctx->argi >= ctx->argc)
		return NULL;

	return ctx->argv[ctx->argi++];
}

static void send_check_cmd(CTX)
{
	int ret;

	if((ret = send_recv_cmd(ctx)) < 0)
		fail(NULL, NULL, ret);
}

/* User commands */

static void cmd_device(CTX)
{
	char *name;
	int ifi;

	if(!(name = shift_arg(ctx)))
		fail("need device name", NULL, 0);

	no_other_options(ctx);

	/* This is suboptimal, but we try to connect first and
	   resolve device name after that. Not worth the trouble
	   messing with the socket to resolve the name first. */
	connect_to_wictl(ctx);

	if((ifi = getifindex(ctx->fd, name)) < 0)
		fail(NULL, name, ifi);

	uc_put_hdr(UC, CMD_WI_SETDEV);
	uc_put_int(UC, ATTR_IFI, ifi);
	uc_put_end(UC);

	send_check_cmd(ctx);
}

static void cmd_status(CTX)
{
	struct ucmsg* msg;

	connect_to_wictl(ctx);

	uc_put_hdr(UC, CMD_WI_STATUS);
	uc_put_end(UC);

	no_other_options(ctx);

	msg = send_recv_aps(ctx);

	dump_status(ctx, msg);
}

static void cmd_neutral(CTX)
{
	struct ucmsg* msg;
	int ret;

	connect_to_wictl(ctx);

	uc_put_hdr(UC, CMD_WI_NEUTRAL);
	uc_put_end(UC);

	no_other_options(ctx);

	if((ret = send_recv_cmd(ctx)) == -ECANCELED)
		return;
	else if(ret < 0)
		fail(NULL, NULL, ret);
	else if(ret > 0)
		fail("unexpected notification", NULL, 0);

	while((msg = recv_reply(ctx)))
		if(msg->cmd == REP_WI_DISCONNECT)
			break;
}

static void resume_service(CTX)
{
	int ret;

	uc_put_hdr(UC, CMD_WI_RESUME);
	uc_put_end(UC);

	if((ret = send_recv_cmd(ctx)) < 0)
		fail(NULL, NULL, ret);
}

static void cmd_resume(CTX)
{
	no_other_options(ctx);
	connect_to_wictl(ctx);

	resume_service(ctx);
}

static void wait_for_scan_results(CTX)
{
	struct ucmsg* msg;
	int* err;

	while((msg = recv_reply(ctx)))
		if(msg->cmd == REP_WI_SCAN_END)
			break;

	if(!msg)
		fail("no reply", NULL, 0); /* should never happen */
	if((err = uc_get_int(msg, ATTR_ERROR)) && *err)
		fail(NULL, NULL, *err);
}

static void fetch_dump_scan_list(CTX)
{
	struct ucmsg* msg;

	uc_put_hdr(UC, CMD_WI_STATUS);
	uc_put_end(UC);

	msg = send_recv_aps(ctx);

	if(msg->cmd < 0)
		fail(NULL, NULL, msg->cmd);

	dump_scanlist(ctx, msg);
}

static void set_scan_device(CTX, int ifi, char* name)
{
	int ret;

	warn("scanning device", name, 0);

	uc_put_hdr(UC, CMD_WI_SETDEV);
	uc_put_int(UC, ATTR_IFI, ifi);
	uc_put_end(UC);

	if((ret = send_recv_cmd(ctx)) < 0)
		fail("cannot set device", name, ret);
}

static void pick_scan_device(CTX)
{
	char name[32];
	int ifi;

	find_wifi_device(name);

	if((ifi = getifindex(ctx->fd, name)) < 0)
		fail(NULL, name, ifi);

	set_scan_device(ctx, ifi, name);
}

static void cmd_scan(CTX)
{
	int ret;

	no_other_options(ctx);

	connect_to_wictl(ctx);

	uc_put_hdr(UC, CMD_WI_SCAN);
	uc_put_end(UC);

	if((ret = send_recv_cmd(ctx)) >= 0)
		goto got;
	else if(ret != -ENODEV)
		fail(NULL, NULL, ret);

	pick_scan_device(ctx);
got:
	wait_for_scan_results(ctx);
	fetch_dump_scan_list(ctx);
}

/* When connecting to a network we don't have PSK for,
   make sure it's range before asking the user for passphrase. */

static void check_suitable_aps(CTX, struct ucmsg* msg)
{
	attr at, scan, ies;
	int seen = 0;

	for(at = uc_get_0(msg); at; at = uc_get_n(msg, at)) {
		if(!(scan = uc_is_nest(at, ATTR_SCAN)))
			continue;
		if(!(ies = uc_sub(scan, ATTR_IES)))
			continue;

		int can = can_use_ap(ctx, ies);

		if(can == AP_CANCONN)
			return;
		if(can == AP_NOCRYPT)
			seen = 1;
	}

	if(seen)
		fail("unsupported encryption mode", NULL, 0);
	else
		fail("no scan results for this SSID", NULL, 0);
}

static void check_ap_in_range(CTX)
{
	struct ucmsg* msg;
	int setdev = 0;
again:
	uc_put_hdr(UC, CMD_WI_STATUS);
	uc_put_end(UC);

	msg = send_recv_aps(ctx);

	if(msg->cmd < 0)
		fail(NULL, NULL, msg->cmd);

	if(uc_get(msg, ATTR_BSSID))
		fail("already connected", NULL, 0);

	if(!uc_get_int(msg, ATTR_IFI)) {
		if(setdev)
			fail("lost active device", NULL, 0);

		pick_scan_device(ctx);
		wait_for_scan_results(ctx);
		setdev = 1;

		goto again;
	} else if(!uc_get(msg, ATTR_SCAN)) {
		if(setdev)
			fail("no APs in range", NULL, 0);

		resume_service(ctx);
		wait_for_scan_results(ctx);
		setdev = 1;

		goto again;
	}

	check_suitable_aps(ctx, msg);
}

static void wait_for_connect(CTX)
{
	struct ucmsg* msg;
	int failures = 0;

	while((msg = recv_reply(ctx))) {
		switch(msg->cmd) {
			case REP_WI_LINK_READY:
				warn_sta(ctx, "connected to", msg);
				return;
			case REP_WI_DISCONNECT:
				warn_bss(ctx, "cannot connect to", msg);
				failures++;
				break;
			case REP_WI_NO_CONNECT:
				if(failures > 1)
					warn("no more APs in range", NULL, 0);
				else if(!failures)
					warn("no suitable APs", NULL, 0);
				_exit(0xFF);
		}
	}
}

static int send_conn_request(CTX)
{
	uc_put_hdr(UC, CMD_WI_CONNECT);
	uc_put_bin(UC, ATTR_SSID, ctx->ssid, ctx->slen);
	uc_put_bin(UC, ATTR_PSK, ctx->psk, sizeof(ctx->psk));
	uc_put_end(UC);

	return send_recv_cmd(ctx);
}

/* When connecting with a saved PSK, no STATUS request is made
   so the service may happen to be in the idle state. */

static void start_conn_scan(CTX)
{
	int ret;

	if((ret = send_conn_request(ctx)) >= 0)
		return;
	if(ret == -ENODEV) {
		pick_scan_device(ctx);
		wait_for_scan_results(ctx);
	} else if(ret == -ENONET) {
		resume_service(ctx);
		wait_for_scan_results(ctx);
	} else {
		goto err;
	}

	if((ret = send_conn_request(ctx)) >= 0)
		return;
err:
	fail(NULL, NULL, ret);

}

/* Connections without PSK always start with a STATUS request
   which should initialize the device, so no point in doing it
   here. */

static void start_connection(CTX)
{
	int ret;

	if((ret = send_conn_request(ctx)) < 0)
		fail(NULL, NULL, ret);
}

/* TODO: SSID parsing.

   SSIDs are not guaranteed to be nice strings, may be almost
   arbitrary byte sequences, up to 32 long. */

static void shift_ssid_arg(CTX)
{
	char *ssid;

	if(!(ssid = shift_arg(ctx)))
		fail("need AP ssid", NULL, 0);

	int slen = strlen(ssid);

	if(slen > 32)
		fail("SSID too long", NULL, 0);

	memcpy(ctx->ssid, ssid, slen);
	ctx->slen = slen;
}

static void cmd_connect(CTX)
{
	shift_ssid_arg(ctx);
	no_other_options(ctx);

	connect_to_wictl(ctx);

	if(load_saved_psk(ctx)) {
		start_conn_scan(ctx);
		wait_for_connect(ctx);
	} else {
		check_ap_in_range(ctx);
		ask_passphrase(ctx);
		start_connection(ctx);
		wait_for_connect(ctx);
		maybe_store_psk(ctx);
	}
}

static void cmd_saved(CTX)
{
	no_other_options(ctx);

	list_saved_psks(ctx);
}

static void cmd_forget(CTX)
{
	shift_ssid_arg(ctx);
	no_other_options(ctx);

	remove_psk_entry(ctx);
}

static void cmd_detach(CTX)
{
	no_other_options(ctx);

	connect_to_wictl(ctx);

	uc_put_hdr(UC, CMD_WI_DETACH);
	uc_put_end(UC);

	send_check_cmd(ctx);
}

typedef void (*cmdptr)(CTX);

static const struct cmdrec {
	char name[12];
	cmdptr call;
} commands[] = {
	{ "device",     cmd_device  },
	{ "detach",     cmd_detach  },
	{ "scan",       cmd_scan    },
	{ "ap",         cmd_connect },
	{ "connect",    cmd_connect },
	{ "dc",         cmd_neutral },
	{ "disconnect", cmd_neutral },
	{ "resume",     cmd_resume  },
	{ "saved",      cmd_saved   },
	{ "forget",     cmd_forget  },
};

static void dispatch(CTX, char* name)
{
	const struct cmdrec* r;

	for(r = commands; r < commands + ARRAY_SIZE(commands); r++)
		if(!strncmp(r->name, name, sizeof(r->name)))
			return r->call(ctx);

	fail("unknown command", name, 0);
}

int main(int argc, char** argv)
{
	struct top context, *ctx = &context;
	char* cmd;

	memzero(ctx, sizeof(ctx));
	init_context(ctx, argc, argv);

	if((cmd = shift_arg(ctx)))
		dispatch(ctx, cmd);
	else
		cmd_status(ctx);

	return 0;
}
