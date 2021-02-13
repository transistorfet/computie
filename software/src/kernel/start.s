
	.global _start
	.global	main
	.global boot_args

	.section .text

/**
 * Image Start
 */

_start:
	| Save boot args from the old stack before re-initializing
	movea.l	(+4,%sp), %a0
	lea	boot_args, %a1
    S1: move.b	(%a0)+, (%a1)+
	bne	S1

	move.l	#0x120000, %sp		| re-initialize the stack pointer to the top of the kernel space (lower 64KB of RAM)
	jsr	main
	stop	#0x2700			| halt, since we clobbered the stack on entry

