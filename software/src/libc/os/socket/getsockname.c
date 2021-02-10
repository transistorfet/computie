
#include <sys/socket.h>
#include <kernel/syscall.h>

int getsockname(int fd, struct sockaddr *addr, socklen_t *len)
{
	volatile unsigned int opts[3] = { SO_GETSOCKNAME, (int) addr, (int) len };
	return SYSCALL3(SYS_GETSOCKOPT, fd, (int) 0, (int) opts);
}


