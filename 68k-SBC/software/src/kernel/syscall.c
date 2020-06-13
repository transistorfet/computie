
#include <stdio.h>

#include "interrupts.h"

#define SYSCALL_MAX	4


void test()
{
	printf("It's the first trap!\n");
}


void *syscall_table[SYSCALL_MAX] = {
	test,
	puts,
	printf,
	putchar,
};

extern void handle_syscall();

void init_syscall()
{
	set_interrupt(IV_TRAP1, handle_syscall);
}


