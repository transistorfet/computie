
#include <stdio.h>

#include <kernel/driver.h>
#include <kernel/filedesc.h>

#include "api.h"
#include "process.h"
#include "interrupts.h"

#define SYSCALL_MAX	10


void *syscall_table[SYSCALL_MAX] = {
	getchar,
	putchar,
	do_read,
	do_write,
	do_fork,
};

extern void enter_syscall();

void init_syscall()
{
	set_interrupt(IV_TRAP1, enter_syscall);
}

// TODO this is temporary until you have processes working correctly
extern struct inode *tty_inode;
extern struct process *current_proc;

size_t do_read(int fd, char *buf, size_t nbytes)
{
	struct file *f = get_fd(current_proc->fd_table, fd);
	if (!f)
		return 0;
	return dev_read(f->inode->device, buf, nbytes);
}


size_t do_write(int fd, const char *buf, size_t nbytes)
{
	struct file *f = get_fd(current_proc->fd_table, fd);
	if (!f)
		return 0;
	return dev_write(f->inode->device, buf, nbytes);
}


int do_fork() {
	// TODO we don't use inodes yet, so passing in NULL
	struct process *proc = new_proc(NULL);
	if (!proc) {
		puts("Ran out of procs\n");
		return;
	}
	int fd = new_fd(proc->fd_table, tty_inode);

	printf("FD: %d\n", fd);


	char *kernel_stack_p;
	asm volatile("move.l	%%sp, (%0)\n" : : "a" (&kernel_stack_p));
	printf("Kernel Stack Address: %x\n", kernel_stack_p);

	size_t kernel_stack_size = 0x200000 - (unsigned int) kernel_stack_p;
	//dump(kernel_stack_p, kernel_stack_size);
	printf("Kernel Stack Size: %x\n", kernel_stack_size);

	int task_size = 0x1000;
	char *task = malloc(task_size);
	char *task_stack_p = task + task_size - kernel_stack_size;
	printf("Task Address: %x\n", task);
	printf("Task Stack Address: %x\n", task_stack_p);
	//memset_s(task, 0, task_size);
	//memcpy_s(task, hello_task, 100);
	//strncpy(task, hello_task, task_size);
	//dump(task, 1000);
	memcpy_s(task_stack_p, kernel_stack_p, kernel_stack_size);

	// insert an empty state onto the stack
	task_stack_p -= 15 * sizeof(void *);
	memset_s(task_stack_p, 0, 15 * sizeof(void *));

	/*
	char *backup;
	swap_stack(&backup, &task_stack_p);
	save_state();
	swap_stack(&task_stack_p, &backup);
	*/

	//puts("Halting...");
	//asm volatile("stop #0x2700\n");

	proc->memory.base = task;
	proc->memory.length = task_size;
	proc->sp = task_stack_p;

	//dump(task_stack_p, kernel_stack_size);

	puts("done");


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
}

