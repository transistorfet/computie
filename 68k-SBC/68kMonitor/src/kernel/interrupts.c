
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "kernel/interrupts.h"


// All data on the stack must be word-aligned, so the stack pointer must be an even
// number, even though there is no memory at this address.  The stack will grow
// downward starting with the word at address 0x1FFFFE
#define STACK_POINTER_INIT	0x200000

extern void _start();
extern void _error();

void handle_error();

const interrupt_handler_t boot_vectors[8] __attribute__((section(".vectors"))) = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	handle_error,
	handle_error,
	handle_error,
	handle_error,
	handle_error,
	handle_error,
};

interrupt_handler_t *vector_table = NULL;

void init_interrupts()
{
	/*
	vector_table = malloc(sizeof(interrupt_handler_t) * 128);

	vector_table[0] = (interrupt_handler_t) STACK_POINTER_INIT;
	vector_table[0] = _start;

	asm("movec	%0, %%vbr\n" : : "r" (vector_table) : );
	*/
}

__attribute__((interrupt)) void handle_error()
{
	puts("Fatal Error: Halting...\n");
	asm("stop #0x2700\n");
}



