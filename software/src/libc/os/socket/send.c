
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t send(int fd, const void *buf, size_t n, int flags)
{
	return SYSCALL5(SYS_SEND, fd, (int) buf, (int) n, flags, 0);
}

