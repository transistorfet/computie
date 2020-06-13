 
#include <kernel/syscall.h>

int putchar(int ch)
{
	SYSCALL(SYS_PUTCHAR, ch, 0, 0);
}
