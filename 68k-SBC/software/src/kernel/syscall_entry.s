
	.global enter_syscall
	.global enter_irq
	.global create_context
	.global save_context
	.global restore_context
	.global _exit

	.global	syscall_table
	.global kernel_stack
	.global current_proc_stack

	.equ	CONTEXT_SIZE, 60		| The size of the saved registers on the stack (not including the interrupt return)

	.section .text


/**
 * System Call Entry Point
 *
 * This is the handler for the system call trap.
 * The system call number (index) is passed in d0, and arguments are passed
 * in order as d1, a0, and a1 (only 3 arguments max atm)
 */
enter_syscall:
	| Disable ints and save the context
	or.w	#0x0700, %sr
	bsr	save_context

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

	| Jump to the syscall interrupt return
	bra	restore_context


/**
 * Serial Controller IRQ
 *
 * This will save the context and stack frame before calling the C function to handle the rest
 */
enter_irq:
	| Disable ints and save the context
	or.w	#0x0700, %sr
	bsr	save_context

	| Move the address of the stack frame into a5
	move.l	current_proc_stack, %a5
	add.l	#CONTEXT_SIZE, %a5
	bsr	handle_serial_irq

	| Jump to the syscall interrupt return
	bra	restore_context


/**
 * Create a new context given a future stack address, return address
 * 	void *create_context(void *user_stack, void *entry);
 */
create_context:
	move.l	(4,%sp), %a0
	move.l	(8,%sp), %a1

	| Push _exit return address for when main returns
	move.l	%a5, -(%sp)
	lea	_exit, %a5
	move.l	%a5, -(%a0)
	move.l	(%sp)+, %a5

	| Push the 68010 Format/Vector Word
	.ifdef __mc68010__
	move.w	#0, -(%a0)
	.endif
	| Push starting address
	move.l	%a1, -(%a0)
	| Push flags
	move.w #0x2000, -(%a0)

	| Push zeros for all registers
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)
	move.l	#0, -(%a0)

	move.l	%a0, %d0
	rts

/**
 * Save all registers to the current user stack and switch to the kernel stack
 */
save_context:
	| Move the return address ahead of where the context will be
	move.l	(%sp), (-CONTEXT_SIZE,%sp)
	add.l	#4, %sp

	| Save registers
	move.l	%a6, -(%sp)
	move.l	%a5, -(%sp)
	move.l	%a4, -(%sp)
	move.l	%a3, -(%sp)
	move.l	%a2, -(%sp)
	move.l	%a1, -(%sp)
	move.l	%a0, -(%sp)
	move.l	%d7, -(%sp)
	move.l	%d6, -(%sp)
	move.l	%d5, -(%sp)
	move.l	%d4, -(%sp)
	move.l	%d3, -(%sp)
	move.l	%d2, -(%sp)
	move.l	%d1, -(%sp)
	move.l	%d0, -(%sp)

	| Save the return address before we switch the stacks
	move.l	(-4,%sp), %a6

	| Switch to the kernel stack
	move.l	%sp, current_proc_stack
	move.l	kernel_stack, %sp

	| Jump to the return address of the caller
	jmp	(%a6)


/**
 * Switch to the current user stack, restore the context, and return from the interrupt
 */
restore_context:
	| Switch back to the current process's stack
	move.l	%sp, kernel_stack
	move.l	current_proc_stack, %sp

	| Restore registers
	move.l	(%sp)+, %d0
	move.l	(%sp)+, %d1
	move.l	(%sp)+, %d2
	move.l	(%sp)+, %d3
	move.l	(%sp)+, %d4
	move.l	(%sp)+, %d5
	move.l	(%sp)+, %d6
	move.l	(%sp)+, %d7
	move.l	(%sp)+, %a0
	move.l	(%sp)+, %a1
	move.l	(%sp)+, %a2
	move.l	(%sp)+, %a3
	move.l	(%sp)+, %a4
	move.l	(%sp)+, %a5
	move.l	(%sp)+, %a6

	rte


_exit:
	| Execute the exit syscall to terminate the current process
	moveq	#1, %d0
	trap	#1

	| This shouldn't run because the syscall should never return
	stop	#0x2000

