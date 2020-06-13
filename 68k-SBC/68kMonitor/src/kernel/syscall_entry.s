/**
 * System Call Entry Point
 *
 * This is the handler for the system call trap.
 * The system call number (index) is passed in d0, and arguments are passed
 * in order as d1, a0, and a1 (only 3 arguments max atm)
 */

	.global handle_syscall
	.global	syscall_table

	.section .text

handle_syscall:
	| Save the syscall number, which we will manipulate
	move.l	%d0, -(%sp)

	| Save the arguments in right to left order to match the C calling convention
	move.l	%a1, -(%sp)
	move.l	%a0, -(%sp)
	move.l	%d1, -(%sp)

	| Calculate the index into the syscall table
	| TODO this is so far the only non-pc-relative address
	lea	syscall_table, %a0	
	lsl.l	#2, %d0
	add.l	%d0, %a0
	move.l	(%a0), %a1

	| Call the C function registered in the syscall table
	jsr	(%a1)

	| Restore the argument registers
	move.l	(%sp)+, %d1
	move.l	(%sp)+, %a0
	move.l	(%sp)+, %a1

	| Restore the syscall number register
	move.l	(%sp)+, %d0
	rte

