
#include <sys/socket.h>
#include <kernel/syscall.h>

ssize_t sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len)
{
	volatile unsigned int opts[4] = { n, flags, (int) addr, addr_len };
	//printf("%d %d, %x, %d\n", opts[0], opts[1], opts[2], opts[3]);
	return SYSCALL3(SYS_SENDTO, fd, (int) buf, (int) opts);
}

