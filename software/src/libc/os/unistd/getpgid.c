
#include <sys/types.h>
#include <kernel/syscall.h>

pid_t getpgid(pid_t pid)
{
	return SYSCALL1(SYS_GETPGID, (int) pid);
}

