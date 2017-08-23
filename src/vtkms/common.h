#include <dirs.h>

#define CONFDIR ETCDIR "/vts"
#define CONTROL RUNDIR "/vtmux"

#define VT(c) TAGGED('V', 'T', c)

#define CMD_STATUS   VT(1)
#define CMD_SWITCH   VT(2)
#define CMD_SPAWN    VT(3)

#define ATTR_VT      1
#define ATTR_TTY    10
#define ATTR_PID    11
#define ATTR_NAME   12
#define ATTR_GRAPH  13
