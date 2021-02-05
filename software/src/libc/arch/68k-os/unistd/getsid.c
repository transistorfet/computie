
#include <unistd.h>
#include <kernel/syscall.h>

pid_t getsid(pid_t pid)
{
	return SYSCALL1(SYS_GETSID, (int) pid);
}

