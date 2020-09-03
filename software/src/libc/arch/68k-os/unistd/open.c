
#include <unistd.h>
#include <kernel/syscall.h>


int open(const char *path, int flags, mode_t mode)
{
	return SYSCALL3(SYS_OPEN, (int) path, (int) flags, (int) mode);
}

