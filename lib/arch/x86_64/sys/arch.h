#include <syscall.h>

#define ARCH_SET_GS 0x1001
#define ARCH_SET_FS 0x1002
#define ARCH_GET_FS 0x1003
#define ARCH_GET_GS 0x1004

inline static long sys_arch_prctl(int code, long arg)
{
	return syscall2(__NR_arch_prctl, code, arg);
}
