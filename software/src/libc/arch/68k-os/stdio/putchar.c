 
#include <unistd.h>
#include <kernel/syscall.h>

int putchar(int ch)
{
	unsigned char c = ch;
	write(0, &c, 1);
	//return SYSCALL1(SYS_TEST, ch);
	return ch;
}