
	.global _start
	.global _error
	.global	main

	.section .text

/**
 * Image Start
 */

_start:

	| Disable interrupts and reset VBR
	move.w	#0x2700, %sr
	.ifdef __mc68010__
	clr	%d0
	movec	%d0, %vbr
	.endif

	|move.w	#0x2000, %sr		| enable all interrupts

	bsr	main
	stop	#0x2700

