
#include <kernel/syscall.h>

int pause()
{
	return SYSCALL1(SYS_PAUSE, (int) 0);
}

