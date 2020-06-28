
	.global _start
	.global	main

	.section .text

/**
 * Image Start
 */

_start:
	move.l	#0x110000, %sp		| re-initialize the stack pointer to the top of the kernel space (lower 64KB of RAM)
	bsr	main
	stop	#0x2700			| halt, since we clobbered the stack on entry

