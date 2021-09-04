
	org	0000h
	jp	start

	org	0080h
	seek	0080h

start:
	ld	sp, 0200h
	call	_main
	halt

_main:
	ld	a, 01h
	out	(00FFh), a

	ld	de, msg

loop:
	ld	a, (de)
	cp	0
	jp	Z, loop_end
	out	(00F1h), a
	inc	de
	jp	loop
	
loop_end:

	ret
; end _main

msg:	defm	"Welcome To Bread80!\n\n\0"


; force the size to 512 bytes
	seek	0200h
	nop
