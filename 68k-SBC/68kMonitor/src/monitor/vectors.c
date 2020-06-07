
#include <stdio.h>

#define STACK_POINTER_INIT	0x200000

extern void _start();
extern void _error();
extern void handle_serial_irq();

void handle_error();

typedef void (*interrupt_handler_t)();

const interrupt_handler_t boot_vectors[8] __attribute__((section(".vectors"))) = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	handle_error,
	handle_error,
	handle_error,
	handle_error,
	handle_error,
	handle_serial_irq,
};

__attribute__((interrupt)) void handle_error()
{
	puts("Fatal Error: Halting...\n");
	asm("stop #0x2700\n");
}


