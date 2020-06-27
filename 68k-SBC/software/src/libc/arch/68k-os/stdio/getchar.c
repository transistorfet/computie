 
#include <stdio.h>
#include <unistd.h>
#include <kernel/syscall.h>

int getchar()
{
	unsigned char ch;
	read(0, &ch, 1);
	return ch;
	//return SYSCALL1(SYS_GETCHAR, 0);
}
