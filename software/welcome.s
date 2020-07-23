
	.org	0x0000

	| Stack Pointer on Reset
	.word	0x0000
	.word	0x1000
	| PC on Reset
	.word	0x0000
	.word	0x0020

	.org	0x0020
_start:

	move.b	#0x01, %d1
	lea	0x02ff, %a1
	move.b	%d1, (%a1)
	|move.b	%d1, 0x02ff


/*
	bsr	test
	stop	#0x2700

test:
	rts
*/

/*
	move.l	#0x65, %d2
	lea	0x02f1, %a2
	move.b	%d2, (%a2)
*/

	|move.l	message_end - message, %d1
	|lea	message, %a1
	lea	0x0050, %a1
	lea	0x02f1, %a2

loop:
	move.b	(%a1)+, %d2
	move.b	%d2, (%a2)

	cmpi.b	#0x00, %d2
	bne	loop


/*
	move.l	#0x0002, %d2
delay:
	subi.w	#0x01, %d2
	cmpi.w	#0x00, %d2
	bne	delay
*/

	stop	#0x2700

	.org	0x0050
message: .ascii	"Welcome to Bread68k!\n\n\0"
message_end:

