
#include <unistd.h>
#include <kernel/syscall.h>

void *sbrk(intptr_t increment)
{
	return (void *) SYSCALL1(SYS_SBRK, (int) increment);
}



