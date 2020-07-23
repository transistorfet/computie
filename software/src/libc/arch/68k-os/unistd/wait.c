
#include <unistd.h>
#include <kernel/syscall.h>


pid_t wait(int *status)
{
	return SYSCALL1(SYS_WAIT, (int) status);
}

