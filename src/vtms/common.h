#include <dirs.h>

#define CONTROL RUN_CTRL "/vtmux"
#define CONFDIR HERE "/etc/vtmux"

#define VT(c) TAGGED('V', 'T', c)

#define CMD_STATUS   VT(1)
#define CMD_SWITCH   VT(2)
#define CMD_SPAWN    VT(3)
#define CMD_SWBACK   VT(4)
#define CMD_SWLOCK   VT(5)
#define CMD_UNLOCK   VT(6)

#define ATTR_VT      1
#define ATTR_TTY    10
#define ATTR_PID    11
#define ATTR_NAME   12
#define ATTR_GRAPH  13
