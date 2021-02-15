
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len)
{
	volatile const void *opts[2] = { addr, (void *) addr_len };
	return SYSCALL5(SYS_SENDTO, fd, (int) buf, n, flags, (int) opts);
}

