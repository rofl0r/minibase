#include <sys/proc.h>

#include <string.h>
#include <format.h>
#include <util.h>

#include "msh.h"

extern const char errtag[];

/* Common quit() and warn() are not very well suited for msh,
   which should preferably use script name and line much more
   often than generic msh: tag, and sometimes maybe even
   impersonate built-in commands. */

static int maybelen(const char* str)
{
	return str ? strlen(str) : 0;
}

/* Cannot use heap here, unless halloc is changed to never cause
   or report errors. */

#define TAGGED_SAVED     0
#define FILE_LINE_SAVED  1
#define FILE_LINE_REDIR  2

static void report(CTX, const char* err, char* arg, long ret, int m)
{
	char* file = ctx->file;
	int line = ctx->line;
	int len = maybelen(file) + maybelen(err) + maybelen(arg) + 50;
	int fd;

	FMTBUF(p, e, buf, len);

	if(m == TAGGED_SAVED) {
		p = fmtstr(p, e, errtag);
	} else if(file) {
		p = fmtstr(p, e, file);
		p = fmtstr(p, e, ":");
		p = fmtint(p, e, line);
	} else {
		p = fmtstr(p, e, errtag);
	}

	p = fmtstr(p, e, ":");

	if(err) {
		p = fmtstr(p, e, " ");
		p = fmtstr(p, e, err);
	} if(arg) {
		p = fmtstr(p, e, " ");
		p = fmtstr(p, e, arg);
	} if((arg || err) && ret) {
		p = fmtstr(p, e, ": ");
	} if(ret) {
		p = fmterr(p, e, ret);
	}

	FMTENL(p, e);

	if(m == FILE_LINE_REDIR)
		fd = STDERR;
	else
		fd = ctx->errfd;

	writeall(fd, buf, p - buf);
}

void exit(CTX, int code)
{
	int ret;

	if(*ctx->trap) {
		char* argv[] = { ctx->trap, NULL };

		ret = sys_execve(*argv, argv, ctx->envp);

		report(ctx, "exec", *argv, ret, TAGGED_SAVED);
	}

	_exit(code);
}

void quit(CTX, const char* err, char* arg, long ret)
{
	report(ctx, err, arg, ret, TAGGED_SAVED);
	exit(ctx, 0xFF);
}

int error(CTX, const char* err, char* arg, long ret)
{
	report(ctx, err, arg, ret, FILE_LINE_REDIR);
	return -1;
}

void fatal(CTX, const char* err, char* arg)
{
	report(ctx, err, arg, 0, FILE_LINE_SAVED);
	exit(ctx, 0xFF);
}

int fchk(long ret, CTX, char* arg)
{
	if(ret < 0)
		return error(ctx, *ctx->argv, arg, ret);
	else
		return 0;
}

/* Arguments handling for builtins */

int numleft(CTX)
{
	return ctx->argc - ctx->argp;
}

int dasharg(CTX)
{
	char* arg = peek(ctx);

	return arg && *arg == '-';
}

int moreleft(CTX)
{
	if(numleft(ctx) > 0)
		return error(ctx, "too many arguments", NULL, 0);
	else
		return 0;
}

int noneleft(CTX)
{
	if(numleft(ctx) <= 0)
		return error(ctx, "too few arguments", NULL, 0);
	else
		return 0;
}

char** argsleft(CTX)
{
	return &(ctx->argv[ctx->argp]);
}

char* peek(CTX)
{
	if(ctx->argp < ctx->argc)
		return ctx->argv[ctx->argp];
	else
		return NULL;
}

char* shift(CTX)
{
	char* arg;

	if((arg = peek(ctx)))
		ctx->argp++;

	return arg;
}

int shift_str(CTX, char** dst)
{
	char* str;

	if(!(str = shift(ctx)))
		return error(ctx, "argument required", NULL, 0);

	*dst = str;
	return 0;
}

static int argument_required(CTX)
{
	return error(ctx, "argument required", NULL, 0);
}

static int numeric_arg_required(CTX)
{
	return error(ctx, "numeric argument required", NULL, 0);
}

int shift_int(CTX, int* dst)
{
	char* p;

	if(!(p = shift(ctx)))
		return argument_required(ctx);
	if(!(p = parseint(p, dst)) || *p)
		return numeric_arg_required(ctx);

	return 0;
}

int shift_u64(CTX, uint64_t* dst)
{
	char* p;

	if(!(p = shift(ctx)))
		return argument_required(ctx);
	if(!(p = parseu64(p, dst)) || *p)
		return numeric_arg_required(ctx);

	return 0;
}

int shift_oct(CTX, int* dst)
{
	char* p;

	if(!(p = shift(ctx)))
		return argument_required(ctx);
	if(*p != '0')
		return error(ctx, "mode must be octal", NULL, 0);
	if(!(p = parseoct(p, dst)) || *p)
		return numeric_arg_required(ctx);

	return 0;
}
