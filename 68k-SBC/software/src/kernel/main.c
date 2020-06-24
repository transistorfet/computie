
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
extern void init_syscall();


extern struct inode *tty_inode;
extern struct process *current_proc;


const char hello_task[400] = {
#include "../test.txt"
};


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

	//print_stack();

	//memset_s(task, 0, 0xC00);		// With memset doing nothing, this value will not cause a fatal
	//memset_s(task, 0, 0xD00);		// With memset doing nothing, this value will sometimes cause a fatal
	//memset_s(task, 0, 0xF00);		// With memset doing nothing, this value will mostly cause a fatal

	//memset_s(task + 0x400, 0, 0x1000);	// TODO this will cause the fatal and the string glitch
	//memset_s(task + 0x800, 0, 0xB00);	// Works but causes the string glitch
	//memset_s(task + 0x400, 0, 0xB00);	// Works but causes the string glitch

	//memset_s(task, 0, task_size);
	memcpy_s(task, hello_task, 400);
	//strncpy(task, hello_task, task_size);
	//dump(task, 1000);

	//task_stack_p -= sizeof(void *);
	//*task_stack_p = task;

	//printf("%d\n", 123456);

	//print_stack();

	asm volatile(
	"move.l	%1, -(%0)\n"
	"move.w #0x2000, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	: "+a" (task_stack_p)
	: "g" (task)
	);

	//TRACE_ON();

	proc->memory.base = task;
	proc->memory.length = task_size;
	proc->sp = task_stack_p;

	//char *buts = "BUTS";
	printf("After: %x\n", task_stack_p);
	//printf("After: %s\n", buts);
	//char buffer[100];
	//itoa(task_stack_p, buffer, 16);
	//puts(buffer);

	//dump(task, 400);


	/*
	asm volatile(
	"move.l	%%sp, %%d2\n"
	"move.l %0, %%sp\n"
	"jsr	(%1)\n"
	"move.l %%d2, %%sp\n"
	: 
	: "r" (task_stack_p), "a" (task)
	: "%d2"
	);
	*/

	//((void (*)()) task)();

	//dump(task, 1000);


	//free(task);
	//free_proc(proc);

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

	asm volatile(
	"move.l	%1, -(%0)\n"
	"move.w #0x2000, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	"move.l	#0, -(%0)\n"
	: "+a" (stack_p)
	: "g" (sh_task)
	);

	proc->memory.base = 0x100000;
	proc->memory.length = 0x10000;
	proc->sp = stack_p;

	printf("After: %x\n", stack_p);

	//dump(task, task_size);



	/*
	asm volatile(
	"move.l	%%sp, %%d2\n"
	"move.l %0, %%sp\n"
	"jsr	(%1)\n"
	"move.l %%d2, %%sp\n"
	: 
	: "r" (task_stack_p), "a" (task)
	: "%d2"
	);
	*/

	//((void (*)()) task)();

	//dump(task, 1000);


	//free(task);
	//free_proc(proc);

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


	// TODO we don't use inodes yet, so passing in NULL
	//current_proc = new_proc(NULL);
	//int fd = new_fd(current_proc->fd_table, tty_inode);


	//struct file *f = get_fd(current_proc->fd_table, fd);
	//if (!f)
	//	return 0;
	//dev_write(f->inode->device, "\nTest\n", 6);



	struct process *task = run_task();
	run_sh();
	//do_fork();

	//for (int i = 0; i < 0x2800; i++)
	//	asm volatile("");

	//printf("THINGS %x\n", current_proc);

	//print_run_queue();

	extern void *kernel_stack;
	extern void *current_proc_stack;

	current_proc = task;
	current_proc_stack = task->sp;
	//kernel_stack = (void *) 0x1F0000;

	//asm("stop #0x2700");

	asm("bra restore_context\n");

	// Start Multitasking
	//ENABLE_INTS();

	//puts("\n\nWelcome to the \x1b[32mOS!\n");
	//dev_write(0, "\n\nWelcome to the \x1b[32mthing!\n", 29);

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

	/*
	int *data = malloc(sizeof(int) * 10);
	data[0] = 0;
	data[1] = 1;
	data[2] = 2;
	printf("%x %x %x\n", &data[0], &data[1], &data[2]);
	printf("%d\n", *(data - 3));
	free(data);

	int *data2 = malloc(4);
	printf("%x %d\n", data2, *(data2 - 3));

	int *data3 = malloc(sizeof(int) * 2);
	printf("%x %d\n", data3, *(data3 - 3));

	free(data2);
	//printf("%x %d\n", data2, *(data2 - 3));
	print_free();

	int *data4 = malloc(sizeof(int) * 10);
	printf("%x %d\n", data4, *(data4 - 3));
	
	print_free();
	*/

	/*
	uint8_t *mem2 = (uint8_t *) 0x100000;
	for (short i = 0; i < 20; i++) {
		printf("%x ", (uint8_t) mem2[i]);
	}
	*/

	/*
	uint16_t *mem3 = (uint16_t *) 0x200000;
	for (int i = 0; i < 0x100; i++) {
		printf("%x ", (uint16_t) mem3[i]);
	}
	*/

	/*
	// Cause an address exception
	char test[10];
	*((short *) &test[1]) = 10;
	*/

	sh_task();

	return 0;
}

