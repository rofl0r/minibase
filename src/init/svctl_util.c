#include <bits/socket/unix.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/file.h>

#include <string.h>
#include <util.h>

#include "common.h"
#include "svctl.h"

void* heap_alloc(CTX, int size)
{
	struct heap* hp = &ctx->hp;

	if(!hp->ptr) {
		void* brk = sys_brk(NULL);
		hp->brk = brk;
		hp->ptr = brk;
		hp->end = brk;
	}

	void* old = hp->ptr;
	void* new = old + size;

	if(new > hp->end) {
		int need = new - hp->end;
		need += (PAGE - need) % PAGE;
		hp->end = sys_brk(hp->end + need);
	}

	if(mmap_error(hp->end) || new > hp->end)
		fail("cannot allocate heap", NULL, 0);

	hp->ptr = new;

	return old;
}

void expect_large(CTX)
{
	int outlen = 1024;
	void* outbuf = heap_alloc(ctx, outlen);

	ctx->bo.fd = STDOUT;
	ctx->bo.buf = outbuf;
	ctx->bo.len = outlen;
	ctx->bo.ptr = 0;

	int rxlen = 1024;
	void* rxbuf = heap_alloc(ctx, rxlen);

	ur_buf_set(&ctx->ur, rxbuf, sizeof(rxbuf));
}

static void extend_urbuf(CTX)
{
	if(ctx->ur.buf == ctx->smallbuf)
		fail("cannot extend small buf", NULL, 0);
	if(ctx->ur.end != ctx->hp.ptr)
		fail("heap misallocation trap", NULL, 0);

	int rxext = 1024;

	(void)heap_alloc(ctx, rxext);

	ctx->ur.end += rxext;
}

void flush_output(CTX)
{
	if(!ctx->bo.ptr)
		return;

	bufoutflush(&ctx->bo);
}

void output(CTX, char* buf, int len)
{
	if(ctx->bo.len)
		bufout(&ctx->bo, buf, len);
	else
		writeall(STDOUT, buf, len);
}

/* Socket init is split in two parts: socket() call is performed early so
   that it could be used to resolve netdev names into ifis, but connection
   is delayed until send_command() to avoid waking up wimon and then dropping
   the connection because of a local error. */

void init_socket(CTX)
{
	int fd;

	if((fd = sys_socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		fail("socket", "AF_UNIX", fd);

	ctx->fd = fd;
}

static void connect_socket(CTX)
{
	int ret;

	struct sockaddr_un addr = {
		.family = AF_UNIX,
		.path = CONTROL
	};

	if((ret = sys_connect(ctx->fd, &addr, sizeof(addr))) < 0)
		fail("connect", addr.path, ret);

	ctx->connected = 1;
}

void start_request(CTX, int cmd, int count, int paylen)
{
	int total = sizeof(struct ucmsg)
		+ count*sizeof(struct ucattr)
		+ paylen + 4*count;

	void* brk;
	int len = sizeof(ctx->smallbuf);

	if(total <= len)
		brk = ctx->smallbuf;
	else
		brk = heap_alloc(ctx, (len = total));

	uc_buf_set(&ctx->uc, brk, len);
	uc_put_hdr(&ctx->uc, cmd);
}

void add_str_attr(CTX, int key, char* val)
{
	uc_put_str(&ctx->uc, key, val);
}

void send_request(CTX)
{
	int wr, fd = ctx->fd;

	uc_put_end(&ctx->uc);

	if(!ctx->connected)
		connect_socket(ctx);

	if((wr = uc_send_whole(fd, &ctx->uc)) < 0)
		fail("write", NULL, wr);

	memzero(&ctx->uc, sizeof(ctx->uc));
}

static void init_small_rxbuf(CTX)
{
	if(ctx->uc.buf == ctx->smallbuf)
		fail("smallbuf tx-locked", NULL, 0);

	void* buf = ctx->smallbuf;
	int len = sizeof(ctx->smallbuf);

	ur_buf_set(&ctx->ur, buf, len);
}

struct ucmsg* recv_reply(CTX)
{
	int ret;

	if(!ctx->ur.buf)
		init_small_rxbuf(ctx);

	while((ret = uc_recv_shift(ctx->fd, &ctx->ur)) < 0)
		if(ret == -ENOBUFS)
			extend_urbuf(ctx);
		else
			fail("recv", NULL, ret);

	return ctx->ur.msg;
}
