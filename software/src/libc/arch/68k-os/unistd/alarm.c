
#include <kernel/syscall.h>

unsigned int alarm(unsigned int seconds)
{
	return SYSCALL1(SYS_ALARM, (int) seconds);
}

