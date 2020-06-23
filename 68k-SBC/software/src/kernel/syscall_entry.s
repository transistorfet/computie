/**
 * System Call Entry Point
 *
 * This is the handler for the system call trap.
 * The system call number (index) is passed in d0, and arguments are passed
 * in order as d1, a0, and a1 (only 3 arguments max atm)
 */

	.global enter_syscall
	.global exit_syscall
	.global	syscall_table
	.global kernel_stack
	.global current_proc_stack

	.section .text

enter_syscall:
	or.w	#0x0700, %sr

	| Save registers
	move.l	%d0, -(%sp)
	move.l	%d1, -(%sp)
	move.l	%d2, -(%sp)
	move.l	%d3, -(%sp)
	move.l	%d4, -(%sp)
	move.l	%d5, -(%sp)
	move.l	%d6, -(%sp)
	move.l	%d7, -(%sp)
	move.l	%a0, -(%sp)
	move.l	%a1, -(%sp)
	move.l	%a2, -(%sp)
	move.l	%a3, -(%sp)
	move.l	%a4, -(%sp)
	move.l	%a5, -(%sp)
	move.l	%a6, -(%sp)

	| Switch to the kernel stack
	move.l	%sp, current_proc_stack
	move.l	kernel_stack, %sp

process_syscall:
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

exit_syscall:
	| Switch back to the current process's stack
	move.l	%sp, kernel_stack
	move.l	current_proc_stack, %sp

	| Restore registers
	move.l	(%sp)+, %a6
	move.l	(%sp)+, %a5
	move.l	(%sp)+, %a4
	move.l	(%sp)+, %a3
	move.l	(%sp)+, %a2
	move.l	(%sp)+, %a1
	move.l	(%sp)+, %a0
	move.l	(%sp)+, %d7
	move.l	(%sp)+, %d6
	move.l	(%sp)+, %d5
	move.l	(%sp)+, %d4
	move.l	(%sp)+, %d3
	move.l	(%sp)+, %d2
	move.l	(%sp)+, %d1
	move.l	(%sp)+, %d0

	rte




	.global enter_irq
	.global exit_irq

enter_irq:
	or.w	#0x0700, %sr

	| Save registers
	move.l	%d0, -(%sp)
	move.l	%d1, -(%sp)
	move.l	%d2, -(%sp)
	move.l	%d3, -(%sp)
	move.l	%d4, -(%sp)
	move.l	%d5, -(%sp)
	move.l	%d6, -(%sp)
	move.l	%d7, -(%sp)
	move.l	%a0, -(%sp)
	move.l	%a1, -(%sp)
	move.l	%a2, -(%sp)
	move.l	%a3, -(%sp)
	move.l	%a4, -(%sp)
	move.l	%a5, -(%sp)
	move.l	%a6, -(%sp)

	| Switch to the kernel stack
	move.l	%sp, current_proc_stack
	move.l	kernel_stack, %sp

process_irq:
	| Move the address of the stack frame into a6
	move.l	#60, %a6
	add.l	current_proc_stack, %a6
	bsr	handle_serial_irq

exit_irq:
	| Switch back to the current process's stack
	move.l	%sp, kernel_stack
	move.l	current_proc_stack, %sp

	| Restore registers
	move.l	(%sp)+, %a6
	move.l	(%sp)+, %a5
	move.l	(%sp)+, %a4
	move.l	(%sp)+, %a3
	move.l	(%sp)+, %a2
	move.l	(%sp)+, %a1
	move.l	(%sp)+, %a0
	move.l	(%sp)+, %d7
	move.l	(%sp)+, %d6
	move.l	(%sp)+, %d5
	move.l	(%sp)+, %d4
	move.l	(%sp)+, %d3
	move.l	(%sp)+, %d2
	move.l	(%sp)+, %d1
	move.l	(%sp)+, %d0

	rte