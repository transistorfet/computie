
#include <sys/socket.h>
#include <kernel/syscall.h>

int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	volatile unsigned int opts[3] = { optname, (int) optval, (int) optlen };
	return SYSCALL3(SYS_SETSOCKOPT, fd, (int) level, (int) opts);
}

