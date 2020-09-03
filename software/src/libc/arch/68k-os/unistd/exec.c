
#include <unistd.h>
#include <kernel/syscall.h>


int exec(const char *path, char *const argv[], char *const envp[])
{
	return SYSCALL3(SYS_EXEC, (int) path, (int) argv, (int) envp);
}

