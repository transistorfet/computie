
#include <sys/socket.h>
#include <kernel/syscall.h>

int accept(int fd, struct sockaddr *addr, socklen_t *addr_len)
{
	return SYSCALL3(SYS_ACCEPT, fd, (int) addr, (int) addr_len);
}

