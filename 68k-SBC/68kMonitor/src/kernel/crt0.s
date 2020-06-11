
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
	|stop	#0x2700
	rts


/**
 * Error Handler
 */
_error:
	move.b	#0x01, %d0
	lea	0x201d, %a0
	move.b	%d0, (%a0)		| set the arduino led as an error indicator

	stop	#0x2700	

