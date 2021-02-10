
#include <sys/socket.h>
#include <kernel/syscall.h>

int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
	volatile unsigned int opts[3] = { optname, (int) optval, (int) optlen };
	return SYSCALL3(SYS_GETSOCKOPT, fd, (int) level, (int) opts);
}

