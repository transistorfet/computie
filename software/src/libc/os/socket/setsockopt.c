
#include <sys/socket.h>
#include <kernel/syscall.h>

int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	return SYSCALL5(SYS_SETSOCKOPT, fd, (int) level, optname, (int) optval, (int) optlen);
}

