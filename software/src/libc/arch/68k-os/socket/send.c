
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t send(int fd, const void *buf, size_t n, int flags)
{
	volatile unsigned int opts[2] = { n, flags };
	return SYSCALL3(SYS_SEND, fd, (int) buf, (int) opts);
}

