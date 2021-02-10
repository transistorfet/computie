
#include <unistd.h>
#include <kernel/syscall.h>


int close(int fd)
{
	return SYSCALL1(SYS_CLOSE, fd);
}

