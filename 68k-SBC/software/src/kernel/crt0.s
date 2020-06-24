
	.global _start
	.global exception_entry
	.global	main
	.global	fatal_error

	.section .text

/**
 * Image Start
 */

_start:
	move.l	#0x200000, %sp		| re-initialize the stack pointer
	bsr	main
	stop	#2700			| halt, since we clobbered the stack on entry

