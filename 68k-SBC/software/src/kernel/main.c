
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/filedesc.h>
#include <kernel/syscall.h>
#include <kernel/driver.h>

#include "interrupts.h"
#include "process.h"


extern void sh_task();


extern void init_tty();

extern struct inode *tty_inode;
extern struct process *current_proc;


const char hello_task[400] = {
#include "../test.txt"
};


void *create_context(void *user_stack, void *entry);

struct process *run_task() {
	// TODO we don't use inodes yet, so passing in NULL
	struct process *proc = new_proc(NULL);
	if (!proc) {
		puts("Ran out of procs\n");
		return;
	}
	int fd = new_fd(proc->fd_table, tty_inode);

	printf("FD: %d\n", fd);


	//for (int i = 0; i < 0x4000; i++)
	//	asm volatile("");

	int task_size = 0x1800;
	char *task = malloc(task_size);
	char *task_stack_p = task + task_size;
	printf("Task Address: %x\n", task);
	printf("Task Stack: %x\n", task_stack_p);

	//memset_s(task, 0, 0xC00);		// With memset doing nothing, this value will not cause a fatal
	//memset_s(task, 0, 0xD00);		// With memset doing nothing, this value will sometimes cause a fatal
	//memset_s(task, 0, 0xF00);		// With memset doing nothing, this value will mostly cause a fatal

	//memset_s(task + 0x400, 0, 0x1000);	// TODO this will cause the fatal and the string glitch
	//memset_s(task + 0x800, 0, 0xB00);	// Works but causes the string glitch
	//memset_s(task + 0x400, 0, 0xB00);	// Works but causes the string glitch

	//memset_s(task, 0, task_size);
	memcpy_s(task, hello_task, 400);
	//dump(task, task_size);

	//print_stack();

 	task_stack_p = create_context(task_stack_p, task);

	proc->memory.base = task;
	proc->memory.length = task_size;
	proc->sp = task_stack_p;

	printf("After: %x\n", task_stack_p);

	return proc;
}

struct process *run_sh()
{
	struct process *proc = new_proc(NULL);
	if (!proc) {
		puts("Ran out of procs\n");
		return;
	}
	int fd = new_fd(proc->fd_table, tty_inode);


	int stack_size = 0x800;
	char *stack = malloc(stack_size);
	char *stack_p = stack + stack_size;
	printf("Sh Bottom: %x\n", stack);
	printf("Sh Stack: %x\n", stack_p);


 	stack_p = create_context(stack_p, sh_task);

	proc->memory.base = 0x100000;
	proc->memory.length = 0x10000;
	proc->sp = stack_p;

	printf("After: %x\n", stack_p);

	//dump(task, task_size);

	return proc;
}

int main()
{
	DISABLE_INTS();


	init_heap((void *) 0x110000, 0x20000);

	init_interrupts();
	init_syscall();
	init_proc();

	init_inode();
	init_tty();


	struct process *task = run_task();
	run_sh();
	//do_fork();

	//print_run_queue();

	//for (int i = 0; i < 0x2800; i++)
	//	asm volatile("");

	//printf("THINGS %x\n", current_proc);


	extern void *current_proc_stack;

	current_proc = task;
	current_proc_stack = task->sp;

	//panic("Panicking for good measure\n");

	// Start Multitasking
	asm("bra restore_context\n");


	// Force an address error
	//volatile uint16_t *data = (uint16_t *) 0x100001;
	//volatile uint16_t value = *data;

	//char *str = "Hey syscall, %d";
	//asm(
	//"move.l	#2, %%d0\n"
	//"move.l	%0, %%d1\n"
	//"move.l	#125, %%a0\n"
	//"trap	#1\n"
	//: : "r" (str)
	//);

}

