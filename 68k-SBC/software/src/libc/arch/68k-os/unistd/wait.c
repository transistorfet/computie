
#include <stddef.h>
#include <kernel/syscall.h>


int wait(int *status)
{
	return SYSCALL1(SYS_WAIT, (int) status);
}

