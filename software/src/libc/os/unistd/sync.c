
#include <kernel/syscall.h>

int sync()
{
	return SYSCALL1(SYS_SYNC, (int) 0);
}

