
#include <kernel/syscall.h>

int ioctl(int fd, unsigned int request, void *argp)
{
	return SYSCALL3(SYS_IOCTL, fd, (int) request, (int) argp);
}

