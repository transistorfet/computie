
	.global _start
	.global _error
	.global	main

/**
 * Vector Table
 */


	.org	0x0000

	| Stack Pointer on Reset
	.word	0x0000
	.word	0x1000
	| PC on Reset
	.word	0x0000
	.word	_start

	| Bus Error
	.word	0x0000
	.word	_error
	| Address Error
	.word	0x0000
	.word	_error
	| Illegal Instruction
	.word	0x0000
	.word	_error
	| Zero Divide
	.word	0x0000
	.word	_error
	| CHK Instruction
	.word	0x0000
	.word	_error
	| TRAPV Instruction
	.word	0x0000
	.word	_error


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
_error:
	move.b	#0x00, %d0
	lea	0x02ff, %a0
	move.b	%d0, (%a0)

	stop	#0x2700	

