
#include <unistd.h>
#include <kernel/syscall.h>


pid_t fork()
{
	return SYSCALL1(SYS_FORK, 0);
}

