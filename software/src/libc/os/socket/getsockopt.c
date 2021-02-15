
#include <sys/socket.h>
#include <kernel/syscall.h>

int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
	return SYSCALL5(SYS_GETSOCKOPT, fd, (int) level, (int) optname, (int) optval, (int) optlen);
}

