
#include <stdio.h>
#include <stdint.h>

#define STACK_POINTER_INIT	0x200000

extern void _start();
extern void _error();
extern void init_tty();
extern void set_leds(uint8_t bits);
extern void dump(const uint16_t *addr, short len);

void fatal_error_entry();

typedef void (*interrupt_handler_t)();

const interrupt_handler_t boot_vectors[16] __attribute__((section(".vectors"))) = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
	fatal_error_entry,
};

asm(
"fatal_error_entry:\n"
//	move.b	#0x01, %d0
//	lea	0x201d, %a0
//	move.b	%d0, (%a0)		| set the arduino led as an error indicator

"	move.l	%a6, -(%sp)\n"
"	move.l	%a5, -(%sp)\n"
"	move.l	%a4, -(%sp)\n"
"	move.l	%a3, -(%sp)\n"
"	move.l	%a2, -(%sp)\n"
"	move.l	%a1, -(%sp)\n"
"	move.l	%a0, -(%sp)\n"
"	move.l	%d7, -(%sp)\n"
"	move.l	%d6, -(%sp)\n"
"	move.l	%d5, -(%sp)\n"
"	move.l	%d4, -(%sp)\n"
"	move.l	%d3, -(%sp)\n"
"	move.l	%d2, -(%sp)\n"
"	move.l	%d1, -(%sp)\n"
"	move.l	%d0, -(%sp)\n"
"\n"
"	move.l	%sp, -(%sp)\n"
"	bsr	fatal_error\n"
"	| stop	#0x2700\n"
"	| Jump to the monitor instead of stopping\n"
"	jmp	0x40\n"
"	rte\n"
);

void fatal_error(uint8_t *sp)
{
	set_leds(0x80);
	init_tty();
	set_leds(0x80);

	printf("\n\nFatal Error with SP: %x\n", sp);
	printf("PC: %x\nStack Dump with Registers:\n", *((uint32_t *) &sp[15 * 4 + 2]));
	dump(sp, 100);
	printf("Resetting...\n");
}


