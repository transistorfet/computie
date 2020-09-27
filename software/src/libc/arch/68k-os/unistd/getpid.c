
#include <sys/types.h>
#include <kernel/syscall.h>


pid_t getpid()
{
	return SYSCALL1(SYS_GETPID, 0);
}

