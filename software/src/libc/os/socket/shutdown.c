
#include <sys/socket.h>
#include <kernel/syscall.h>

int shutdown(int fd, int how)
{
	return SYSCALL2(SYS_SHUTDOWN, fd, (int) how);
}

