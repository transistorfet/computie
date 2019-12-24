
	.global _start
	.global	main

	.org	0x0000

/**
 * Vector Table
 */
	| Stack Pointer on Reset
	.word	0x0000
	.word	0x1000
	| PC on Reset
	.word	0x0000
	.word	0x0020

	.word	0x0000
	.word	0x0030
	.word	0x0000
	.word	0x0030
	.word	0x0000
	.word	0x0030
	.word	0x0000
	.word	0x0030
	.word	0x0000
	.word	0x0030
	.word	0x0000
	.word	0x0030

/**
 * Image Start
 */
	.org	0x0020
_start:

	bsr	main
	stop	#0x2700


/**
 * Error Handler
 */
	.org	0x0030
_error:
	move.b	#0x00, %d0
	lea	0x02ff, %a0
	move.b	%d0, (%a0)

	stop	#0x2700	

