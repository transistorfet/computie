
#include <unistd.h>
#include <kernel/syscall.h>

int brk(void *addr)
{
	return SYSCALL1(SYS_BRK, (int) addr);
}



