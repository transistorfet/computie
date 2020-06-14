 
#include <kernel/syscall.h>

int putchar(int ch)
{
	return SYSCALL1(SYS_PUTCHAR, ch);
	//return SYSCALL3(SYS_PUTCHAR, ch, 0, 0);
}
