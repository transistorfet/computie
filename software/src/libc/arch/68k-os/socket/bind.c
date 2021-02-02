
#include <sys/socket.h>
#include <kernel/syscall.h>

int bind(int fd, const struct sockaddr *addr, socklen_t len)
{
	return SYSCALL3(SYS_BIND, fd, (int) addr, (int) len);
}

