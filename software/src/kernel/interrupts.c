
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <kernel/printk.h>

#include "interrupts.h"


// TODO this isn't actually used by anything, since a reset will always return to the eeprom's memory (VBR=0)
// All data on the stack must be word-aligned, so the stack pointer must be an even
// number, even though there is no memory at this address.  The stack will grow
// downward starting with the word at address 0x1FFFFE
#define STACK_POINTER_INIT	0x200000

#define INTERRUPT_MAX		128

extern void _start();
extern void exception_entry();
extern void tty_68681_tx_safe_mode();

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
	asm volatile("movec	%0, %%vbr\n" : : "r" (vector_table));
}

void set_interrupt(char iv_num, interrupt_handler_t handler)
{
	vector_table[iv_num] = handler;
}

__attribute__((noreturn)) void panic(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vprintk(fmt, args);
	va_end(args);

	asm("stop #0x2700\n");
	__builtin_unreachable();
}


/**
 * Interrupt Handlers
 */

struct exception_stack_frame {
	uint16_t status;
	uint16_t *pc;
	uint16_t vector;
};

#define INTERRUPT_ENTRY(name)				\
__attribute__((noreturn)) void enter_##name()		\
{							\
	asm(						\
	"move.l	%sp, %a5\n"				\
	"bra	" #name "\n"				\
	);						\
	__builtin_unreachable();			\
}

#define GET_FRAME(frame_ptr)				\
	asm("move.l	%%a5, %0\n" : "=r" (frame_ptr))


INTERRUPT_ENTRY(fatal_error);

__attribute__((interrupt)) void fatal_error()
{
	DISABLE_INTS();

	struct exception_stack_frame *frame;

	GET_FRAME(frame);

	tty_68681_tx_safe_mode();
	printk("\n\nFatal Error at %x (status: %x, vector: %x). Halting...\n", frame->pc, frame->status, (frame->vector & 0xFFF) >> 2);

	char *sp;
	asm volatile("move.l  %%sp, %0\n" : "=r" (sp));

	// Dump stack
	printk("Stack: %x\n", sp);
	for (char i = 0; i < 48; i++) {
		printk("%04x ", ((uint16_t *) frame)[i]);
		if ((i & 0x7) == 0x7)
			printk("\n");
	}

	// Dump code where the error occurred
	printk("\nCode:\n");
	for (char i = 0; i < 48; i++) {
		printk("%04x ", frame->pc[i]);
		if ((i & 0x7) == 0x7)
			printk("\n");
	}

	// Jump to the monitor to allow debugging
	asm volatile(
	"move.l	#0, %a0\n"
	"movec	%a0, %vbr\n"
	//"move.l	(%a0)+, %sp\n"		// TODO it's safer to reset the stack pointer for the monitor, but if we keep the old one, it's easier to debug fatals
	"move.l	#4, %a0\n"
	"jmp	(%a0)\n"
	);

	asm volatile("stop #0x2700\n");
}

INTERRUPT_ENTRY(handle_trace);

__attribute__((interrupt)) void handle_trace()
{
	struct exception_stack_frame *frame;

	GET_FRAME(frame);

	printk("Trace %x (%x)\n", frame->pc, *frame->pc);
}
