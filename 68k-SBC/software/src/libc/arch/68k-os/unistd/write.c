
#include <stddef.h>
#include <kernel/syscall.h>


size_t write(int fd, const char *buf, size_t nbytes)
{
	return SYSCALL3(SYS_WRITE, fd, (void *) buf, (void *) nbytes);
}
