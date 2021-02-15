
#include <sys/socket.h>
#include <kernel/syscall.h>

int getsockname(int fd, struct sockaddr *addr, socklen_t *len)
{
	return SYSCALL5(SYS_GETSOCKOPT, fd, (int) 0, SO_GETSOCKNAME, (int) addr, (int) len);
}


