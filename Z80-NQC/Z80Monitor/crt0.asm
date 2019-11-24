
	.module crt0
	.globl    _main

	.globl    l__INITIALIZER
	.globl    s__INITIALIZER
	.globl    s__INITIALIZED

	.area    _HEADER (ABS)
	.org	0x0000
	nop		; this is to avoid a faulty (0x0000) byte
	jp	init

	.org    0x08
	reti
	.org    0x10
	reti
	.org    0x18
	reti
	.org    0x20
	reti
	.org    0x28
	reti
	.org    0x30
	reti
	.org    0x38

	.org	0x00080

init:
	ld	sp, #0x0FFF
	call	gsinit
	call	_main
	halt

	.area    _HOME
	.area    _CODE
	.area    _INITIALIZER
	.area   _GSINIT
	.area   _GSFINAL

	.area    _DATA
	.area    _INITIALIZED
	.area    _BSEG
	.area   _BSS
	.area   _HEAP

	.area   _CODE

	.area   _GSINIT
gsinit:
	ld    bc, #l__INITIALIZER
	ld    a, b
	or    a, c
	jr    Z, gsinit_next
	ld    de, #s__INITIALIZED
	ld    hl, #s__INITIALIZER
	ldir
gsinit_next:
	.area   _GSFINAL
	ret



