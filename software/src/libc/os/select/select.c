
#include <sys/select.h>
#include <kernel/syscall.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	return SYSCALL5(SYS_SELECT, nfds, (int) readfds, (int) writefds, (int) exceptfds, (int) timeout);
}

