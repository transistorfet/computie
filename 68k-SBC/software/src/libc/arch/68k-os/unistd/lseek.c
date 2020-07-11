
#include <unistd.h>
#include <kernel/syscall.h>


offset_t lseek(int fd, offset_t offset, int whence)
{
	return SYSCALL3(SYS_LSEEK, fd, (void *) offset, (void *) whence);
}

