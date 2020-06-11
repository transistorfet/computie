
#include "interrupts.h"

__attribute__((interrupt)) void handle_syscall()
{
	char *str;

	asm("move.l	%%d0, %0\n": "=r" (str));
	puts(str);
	printf("It's the first trap!\n");

}

void init_syscall()
{
	set_interrupt(IV_TRAP1, handle_syscall);
}


