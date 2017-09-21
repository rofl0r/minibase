#include <bits/ints.h>

/* arguments for hset/hrev */
#define HEAP 0
#define ESEP 1
#define CSEP 2
#define VSEP 3

/* env entry types */
#define ENVDEL 0
#define ENVPTR 1
#define ENVSTR 2
#define ENVLOC 3

/* Heap layout, at the point when end_cmd() calls exec():

   heap                csep                           hend
   v                   v                              v
   Ep Ep Ep Es Es ENVP Arg Arg Arg Arg ARGV ..........|
                  ^                         ^
                  esep                      hptr

   Ep = struct envptr
   Es = struct env with inline payload
   ENVP = char** envp pointing back to Es-s and/or following Ep-s
   Arg = raw 0-terminated string
   ARGV = char** argv pointing back to Arg-s

   Until the first env change, esep=NULL, csep=heap and sh.envp
   points to the original main() argument. */

struct sh {
	char* file;      /* for error reporting */
	int line;
	int errfd;       /* stderr, dup'ed if necessary */

	char** envp;

	int topargc;     /* for the script itself */
	int topargp;
	char** topargv;

	int state;       /* of the parser */
	int argc;
	char** argv;     /* the command being parsed */
	int argp;

	char* heap;      /* layout scheme above */
	char* esep;
	char* csep;
	char* hptr;
	char* hend;
	char* var;       /* heap ptr to $var being substituted */

	int dash;        /* leading - to suppress abort-on-failure */

	char pid[20];

	char trap[50];   /* see cmd_onerror() */
};

struct env {
	unsigned short len;
	char type;
	char payload[];
};

struct envptr {
	unsigned short len;
	char type;
	char* ref;
};

struct mbuf {
	char* buf;
	int len;
};

#define CTX struct sh* ctx

void hinit(CTX);
void* halloc(CTX, int len);
void hrev(CTX, int type);
void hset(CTX, int what);

void parse(CTX, char* buf, int len);
void pfini(CTX);

void loadvar(CTX, char* var);
char* valueof(CTX, char* var);
void setenv(CTX, char* pkey, char* pval);
void define(CTX, char* var, char* val);
void undef(CTX, char* var);
int export(CTX, char* var);

void command(CTX);

#define NR __attribute__((noreturn))
void quit(CTX, const char* err, char* arg, long ret) NR;
int error(CTX, const char* err, char* arg, long ret);
void fatal(CTX, const char* err, char* arg) NR;
int fchk(long ret, CTX, char* arg);

int numleft(CTX);
int dasharg(CTX);
int moreleft(CTX);
int noneleft(CTX);
char** argsleft(CTX);
char* peek(CTX);
char* shift(CTX);
int shift_str(CTX, char** dst);
int shift_int(CTX, int* dst);
int shift_u64(CTX, uint64_t* dst);
int shift_oct(CTX, int* dst);

int mmapfile(struct mbuf* mb, char* name);
int munmapfile(struct mbuf* mb);
