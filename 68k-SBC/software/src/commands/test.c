 
#include <stdio.h>

#include <unistd.h>
#include <kernel/syscall.h>


int main()
{
	const char *msg = "Hello !\n";

	puts("Hello, World!");
	//write(0, msg, 14);
	//putchar('#');
	//SYSCALL1(SYS_PUTCHAR, '!');
	//SYSCALL3(SYS_WRITE, 0, "Hello, World!\n", 14);
	//SYSCALL3(SYS_WRITE, 0, msg, 14);

	return 0;
}

