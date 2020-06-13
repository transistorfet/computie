
#include <stdio.h>
#include <stdint.h>

#define STACK_POINTER_INIT	0x200000

extern void _start();
extern void _error();
extern void handle_serial_irq();

void fatal_error();

typedef void (*interrupt_handler_t)();

const interrupt_handler_t boot_vectors[8] __attribute__((section(".vectors"))) = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	fatal_error,
	fatal_error,
	fatal_error,
	fatal_error,
	fatal_error,
	handle_serial_irq,
};

__attribute__((interrupt)) void fatal_error()
{
	uint32_t *addr;
	asm("move.l	%%sp,%0\n" : "=r" (addr));
	printf("Fatal Error at %x, %x: Halting...\n", *addr, *(addr + 1));
	asm("stop #0x2700\n");
}


