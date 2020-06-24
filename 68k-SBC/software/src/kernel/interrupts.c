
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
void handle_trace();

static interrupt_handler_t vector_table[INTERRUPT_MAX] = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
};

void init_interrupts()
{
	extern void enter_fatal_error();
	for (short i = 2; i < INTERRUPT_MAX; i++)
		vector_table[i] = enter_fatal_error;

	extern void enter_handle_trace();
	set_interrupt(IV_TRACE, enter_handle_trace);

	// Load the VBR register with the address of our vector table
	asm("movec	%0, %%vbr\n" : : "r" (vector_table));
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


#define INTERRUPT_ENTRY(name)				\
__attribute__((naked, noreturn)) void enter_##name()	\
{							\
	asm(						\
	"move.l	%sp, %a5\n"				\
	"bra	" #name "\n"				\
	);						\
}

INTERRUPT_ENTRY(fatal_error);

__attribute__((interrupt)) void fatal_error()
{
	DISABLE_INTS();

	struct exception_stack_frame *frame;
	asm("move.l	%%a5, %0\n" : "=r" (frame));	// NOTE the exception_entry function pushes %sp into %a5 and then jumps here
	printf("Fatl Error at %x (status: %x, vector: %x). Halting...\n", frame->pc, frame->status, (frame->vector & 0xFFF) >> 2);

	char *sp;
	asm volatile("move.l  %%sp, %0\n" : "=r" (sp));
	printf("SP: %x\n", sp);

	// Dump code where the error occurred
	for (char i = 0; i < 12; i++) {
		printf("%x ", frame->pc[i]);
		if (i & 0x3 == 0x3)
			putchar('\n');
	}

	// Jump to the monitor to allow debugging
	asm(
	"move.l	#0, %a0\n"
	"movec	%a0, %vbr\n"
	//"move.l	(%a0)+, %sp\n"
	"move.l	#4, %a0\n"
	"jmp	(%a0)\n"
	);

	asm("stop #0x2700\n");
}

INTERRUPT_ENTRY(handle_trace);

__attribute__((interrupt)) void handle_trace()
{
	struct exception_stack_frame *frame;
	asm("move.l	%%a5, %0\n" : "=r" (frame));	// NOTE the exception_entry function pushes %sp into %a5 and then jumps here
	printf("Trace %x (%x)\n", frame->pc, *frame->pc);
}
