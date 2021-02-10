
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t recv(int fd, void *buf, size_t n, int flags)
{
	volatile unsigned int opts[2] = { n, flags };
	return SYSCALL3(SYS_RECV, fd, (int) buf, (int) opts);
}

