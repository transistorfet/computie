
	| Definitions in this file
	.global enter_syscall
	.global enter_irq
	.global create_context
	.global drop_context
	.global save_context
	.global restore_context
	.global _exit
	.global _sigreturn

	| Declarations defined outside of this file
	.global	syscall_table
	.global kernel_reentries
	.global kernel_stack
	.global current_proc
	.global current_syscall
	.global do_syscall
	.global need_reschedule
	.global schedule
	.global bh_requested
	.global run_bh_handlers

	.equ	CONTEXT_SIZE, 60		| The size of the saved registers on the stack (not including the interrupt return)
	.equ	PROC_SP_OFFSET, 8		| The offset into struct process where the stack pointer is located

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

	| Save the arguments in the order expected of the syscall struct
	move.l	%d0, -(%sp)
	move.l	%d3, -(%sp)
	move.l	%d2, -(%sp)
	move.l	%a1, -(%sp)
	move.l	%a0, -(%sp)
	move.l	%d1, -(%sp)

	move.l	%sp, current_syscall

	| Enable interrupts during the syscall
	and.w	#0xF8FF, %sr
	jsr	do_syscall
	or.w	#0x0700, %sr

	| Restore the syscall registers
	move.l	(%sp)+, %d1
	move.l	(%sp)+, %a0
	move.l	(%sp)+, %a1
	move.l	(%sp)+, %d2
	move.l	(%sp)+, %d3
	move.l	(%sp)+, %d0

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

	bsr	handle_serial_irq

	| Jump to the syscall interrupt return
	bra	restore_context


/**
 * Create a new context given a future stack address, return address
 * 	void *create_context(void *user_stack, void *entry, void *exit);
 */
create_context:
	move.l	(4,%sp), %a0
	move.l	(8,%sp), %a1
	move.l	(12,%sp), %d0

	| Push _exit return address for when main returns
	move.l	%d0, -(%a0)

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
 * Remove the context from the stack and return the new stack pointer
 * 	void *drop_context(void *user_stack);
 */
drop_context:
	move.l	(4,%sp), %a0
	add.l	#CONTEXT_SIZE, %a0
	.ifdef __mc68010__
	add.l	#2, %a0
	.endif
	add.l	#6, %a0
	move.l	%a0, %d0
	rts

/**
 * Save all registers to the current user stack and switch to the kernel stack
 */
save_context:
	| Increment the number of times we've entered the kernel, and skip the stack switch if we're already inside
	add.l	#1, kernel_reentries
	cmp.l	#1, kernel_reentries
	beq	L001

	| We're already on the kernel stack, so just save the scratch registers and return
	move.l	(%sp), (-16,%sp)
	add.l	#4, %sp
	move.l	%a1, -(%sp)
	move.l	%a0, -(%sp)
	move.l	%d1, -(%sp)
	move.l	%d0, -(%sp)
	sub.l	#4, %sp
	rts

    L001:
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
	move.l	current_proc, %a5
	adda.l	#PROC_SP_OFFSET, %a5
	move.l	%sp, (%a5)
	move.l	kernel_stack, %sp

	| Jump to the return address of the caller
	jmp	(%a6)


/**
 * Switch to the current user stack, restore the context, and return from the interrupt
 */
restore_context:
	| If we aren't exiting to a user process, then skip the next few checks
	cmp.l	#1, kernel_reentries
	bne	L004

    L002:

	| Check if there are any back half handlers to be run
	cmp.l	#0, bh_requested
	beq	L003

	| Enable interrupts while running the back half handlers
	and.w	#0xF8FF, %sr
	bsr	run_bh_handlers
	or.w	#0x0700, %sr

    L003:

	| Check if there was a request to schedule a different process
	cmp.l	#0, need_reschedule
	beq	L004

	| Enable interrupts while running the scheduler
	and.w	#0xF8FF, %sr
	bsr	schedule
	or.w	#0x0700, %sr

	| After a process has been scheduled and a syscall restarted, it might need to be suspended right away again, so loop over the checks
	bra	L002

    L004:

	| Decrement the number of times we've entered the kernel, and if this is the last time, then switch to the user stack
	sub.l	#1, kernel_reentries
	beq	L005

	| We'll still be on the kernel stack after returning, so just pop off the saved scratch register values and return
	move.l	(%sp)+, %d0
	move.l	(%sp)+, %d1
	move.l	(%sp)+, %a0
	move.l	(%sp)+, %a1
	rte

    L005:

	| Switch back to the current process's stack
	move.l	%sp, kernel_stack
	move.l	current_proc, %a5
	adda.l	#PROC_SP_OFFSET, %a5
	move.l	(%a5), %sp

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

_sigreturn:
	| Execute the sigreturn syscall to clean up the signal handling
	moveq	#30, %d0
	trap	#1

	| This shouldn't run because the syscall should never return
	stop	#0x2000

