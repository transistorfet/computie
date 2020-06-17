
	.global _start
	.global exception_entry
	.global	main
	.global	fatal_error

	.section .text

/**
 * Image Start
 */

_start:

	move.w	#0x2000, %sr		| enable all interrupts
	bsr	main
	|stop	#0x2700
	rts				| return to the monitor

/**
 * Error Handler
 */
exception_entry:
	move.l	%sp, %a5
	| TODO this is the second absolute reference that prevents the kernel from being relocatable
	jmp	fatal_error

