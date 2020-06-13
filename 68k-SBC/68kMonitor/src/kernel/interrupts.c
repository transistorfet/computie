
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "interrupts.h"


// All data on the stack must be word-aligned, so the stack pointer must be an even
// number, even though there is no memory at this address.  The stack will grow
// downward starting with the word at address 0x1FFFFE
#define STACK_POINTER_INIT	0x200000

#define INTERRUPT_MAX		128

extern void _start();
extern void _error();

void fatal_error();
void handle_trap_1();

static interrupt_handler_t vector_table[INTERRUPT_MAX] = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	fatal_error,	// Bus Error
	fatal_error,	// Address Error
	fatal_error,	// Illegal Instruction
	fatal_error,	// Zero Divide
	fatal_error,	// CHK Instruction
	fatal_error,	// TRAPV Instruction
	fatal_error,	// Privilege Violation
	NULL,		// Trace
};

void init_interrupts()
{
	// Load the VBR register with the address of our vector table
	asm(
	"movec	%0, %%vbr\n"
	: : "r" (vector_table) :
	);
}

void set_interrupt(char iv_num, interrupt_handler_t handler)
{
	vector_table[iv_num] = handler;
}

__attribute__((interrupt)) void fatal_error()
{
	uint16_t *addr;
	asm("move.l	%%sp,%0\n" : "=r" (addr));
	printf("Fatl Error at %x, %x, %x, %x, %x, %x: Halting...\n", *addr, *(addr + 1), *(addr + 2), *(addr + 3), *(addr + 4), *(addr + 5));
	asm("stop #0x2700\n");
}

