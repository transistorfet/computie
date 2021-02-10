
#include <sys/socket.h>
#include <kernel/syscall.h>

int socket(int domain, int type, int protocol)
{
	return SYSCALL3(SYS_SOCKET, domain, (int) type, (int) protocol);
}

