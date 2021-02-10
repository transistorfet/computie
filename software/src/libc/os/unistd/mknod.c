

#include <sys/types.h>
#include <kernel/syscall.h>

int mknod(const char *path, mode_t mode, device_t dev)
{
	return SYSCALL3(SYS_MKNOD, (int) path, (int) mode, (int) dev);
}

