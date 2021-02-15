
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t recv(int fd, void *buf, size_t n, int flags)
{
	return SYSCALL5(SYS_RECV, fd, (int) buf, (int) n, flags, 0);
}

