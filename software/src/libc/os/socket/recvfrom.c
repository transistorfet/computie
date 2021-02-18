
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t recvfrom(int fd, void *buf, size_t n, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
	volatile int opts[2] = { (int) addr, (int) addr_len };
	return SYSCALL5(SYS_RECVFROM, fd, (int) buf, n, flags, (int) opts);
}

