
	.global _start
	.global _error
	.global	main

/**
 * Vector Table
 */


	.org	0x0000

	| Stack Pointer on Reset
	.word	0x0010
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

	move.w	#0x2000, %sr		| enable all interrupts
	bsr	main
	stop	#0x2700


/**
 * Error Handler
 */
_error:
	move.b	#0x01, %d0
	lea	0x201d, %a0
	move.b	%d0, (%a0)		| set the arduino led as an error indicator

	stop	#0x2700	

