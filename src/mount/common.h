#include <dirs.h>

#define CONTROL RUN_CTRL "/mount"

#define MT(c) TAGGED('M', 'T', c)

#define CMD_MOUNT      MT(1)
#define CMD_MOUNT_FD   MT(2)
#define CMD_UMOUNT     MT(3)
#define CMD_GRAB       MT(4)
#define CMD_RELEASE    MT(6)

#define ATTR_NAME     1
#define ATTR_PATH     2
#define ATTR_RDONLY   3
#define ATTR_ERROR    4
