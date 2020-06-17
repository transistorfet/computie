
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
extern void exception_entry();

void fatal_error();
void handle_trap_1();

static interrupt_handler_t vector_table[INTERRUPT_MAX] = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	exception_entry,	// Bus Error
	exception_entry,	// Address Error
	exception_entry,	// Illegal Instruction
	exception_entry,	// Zero Divide
	exception_entry,	// CHK Instruction
	exception_entry,	// TRAPV Instruction
	exception_entry,	// Privilege Violation
	NULL,			// Trace
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


struct exception_stack_frame {
	uint16_t status;
	uint16_t *pc;
	uint16_t vector;
};

__attribute__((interrupt)) void fatal_error()
{
	struct exception_stack_frame *frame;
	asm("move.l	%%a5, %0\n" : "=r" (frame));	// NOTE the exception_entry function pushes %sp into %a5 and then jumps here
	printf("Fatl Error at %x (status: %x, vector: %x). Halting...\n", frame->pc, frame->status, frame->vector);

	// Dump code where the error occurred
	for (char i = 0; i < 12; i++) {
		printf("%x ", frame->pc[i]);
		if (i & 0x3 == 0x3)
			putchar('\n');
	}

	asm("stop #0x2700\n");
}

