
mem_addr:	equ	2000h
mem_size:	equ	2000h

	org	0000h
	jp	start

	org	0080h
	seek	0080h

start:

_main:

	; Initialize error count to 0
	ld	a, 00h
	ex	af, af'

	; Set LED on
	ld	a, 01h
	ld	(00FFh), a

	; Write data to memory
	ld	a, 01h
	ld	bc, mem_size
	ld	hl, mem_addr

write:
	ld	(hl), a
	inc	hl
	inc	a
	dec	bc
	jp	NZ, write

	; Read data from memory
	ld	d, 01h
	ld	bc, mem_size
	ld	hl, mem_addr
read:
	ld	a, (hl)
	cp	d
	jp	NZ, set_error
ret_error:
	inc	hl
	inc	d
	dec	bc
	jp	NZ, read
	
loop_end:

	ex	af, af'
	cp	0
	jp	Z, skip
	ld	a, 0
	ld	(00FFh), a
skip:
	halt

set_error:
	ex	af, af'
	inc	a
	ld	(00FEh), a
	ex	af, af'
	jp	ret_error

; end _main

; force the size to 512 bytes
	seek	0200h
	nop

