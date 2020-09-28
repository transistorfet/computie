
#include <unistd.h>
#include <kernel/syscall.h>


int execve(const char *path, char *const argv[], char *const envp[])
{
	return SYSCALL3(SYS_EXECVE, (int) path, (int) argv, (int) envp);
}

