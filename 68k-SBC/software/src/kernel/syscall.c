
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


int do_fork()
{
	// TODO we don't use inodes yet, so passing in NULL
	struct process *proc = new_proc(NULL);
	if (!proc)
		panic("Ran out of procs\n");
	int fd = new_fd(proc->fd_table, tty_inode);

	printf("FD: %d\n", fd);

	int task_size = 0x1000;
	char *task = malloc(task_size);
	char *task_stack_p = task + task_size;
	printf("Task Address: %x\n", task);
	printf("Task Stack Address: %x\n", task_stack_p);

	//memcpy_s(task, hello_task, 400);
	//dump(task, task_size);

	//print_stack();

 	task_stack_p = create_context(task_stack_p, task);

	proc->memory.base = task;
	proc->memory.length = task_size;
	proc->sp = task_stack_p;

	return 0;
}

int do_exit()
{
	puts("Exiting process");

	free(current_proc->memory.base);
	free_proc(current_proc);

	return 0;
}

