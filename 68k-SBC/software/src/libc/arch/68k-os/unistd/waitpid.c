
#include <unistd.h>
#include <kernel/syscall.h>


pid_t waitpid(pid_t pid, int *status, int options)
{
	return SYSCALL3(SYS_WAITPID, pid, status, options);
}

