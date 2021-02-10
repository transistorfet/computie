
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t recvfrom(int fd, void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t *addr_len)
{
	volatile unsigned int opts[4] = { n, flags, (int) addr, (int) addr_len };
	return SYSCALL3(SYS_RECVFROM, fd, (int) buf, (int) opts);
}

