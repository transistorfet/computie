
	.global _start
	.global _error
	.global	main

	.section .text

/**
 * Image Start
 */

_start:

	move.w	#0x2000, %sr		| enable all interrupts
	bsr	main
	stop	#0x2700

