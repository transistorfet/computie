
#include <stddef.h>
#include <kernel/syscall.h>


int fork()
{
	return SYSCALL1(SYS_FORK, 0);
}


