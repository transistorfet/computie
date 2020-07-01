
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <kernel/driver.h>
#include <kernel/filedesc.h>

#include "api.h"
#include "process.h"
#include "interrupts.h"

#define SYSCALL_MAX	10

void test() { puts("It's a test!\n"); }

void *syscall_table[SYSCALL_MAX] = {
	test,
	do_exit,
	do_fork,
	do_read,
	do_write,
	do_open,
	do_close,
};

extern void enter_syscall();

void init_syscall()
{
	set_interrupt(IV_TRAP1, enter_syscall);
}

// TODO this is temporary until you have processes working correctly
extern struct vnode *tty_vnode;
extern struct process *current_proc;
extern void *current_proc_stack;

void *create_context(void *user_stack, void *entry);


int do_open(const char *path, int oflags)
{
	int fd;
	struct vnode *vnode;

	if (!strcmp(path, "/dev/tty"))
		vnode = tty_vnode;
	else
		return -ENOENT;

	fd = new_fd(current_proc->fd_table, vnode);
	return fd;
}

int do_close(int fd)
{
	free_fd(current_proc->fd_table, fd);
	return 0;
}

size_t do_read(int fd, char *buf, size_t nbytes)
{
	struct file *f = get_fd(current_proc->fd_table, fd);
	if (!f)
		return -1;
	return dev_read(f->vnode->device, buf, nbytes);
}


size_t do_write(int fd, const char *buf, size_t nbytes)
{
	struct file *f = get_fd(current_proc->fd_table, fd);
	if (!f)
		return -1;
	return dev_write(f->vnode->device, buf, nbytes);
}

int do_fork()
{
	struct process *proc;

	proc = new_proc();
	if (!proc)
		panic("Ran out of procs\n");
	// TODO this is only here because we don't clone the fd table
	new_fd(proc->fd_table, tty_vnode);

	// Save the current process's stack pointer back to it's struct, which
	// normally only happens in the schedule() function
	current_proc->sp = current_proc_stack;

	int stack_size = current_proc->segments[S_STACK].length;
	char *stack = malloc(stack_size);
	char *stack_pointer = (stack + stack_size) - ((current_proc->segments[S_STACK].base + current_proc->segments[S_STACK].length) - current_proc->sp);

	memcpy_s(stack, current_proc->segments[S_STACK].base, current_proc->segments[S_STACK].length);

	printf("Parent Stack Pointer: %x\n", current_proc->sp);

	printf("Fork Stack: %x\n", stack);
	printf("Fork Stack Top: %x\n", stack + stack_size);
	printf("Fork Stack Pointer: %x\n", stack_pointer);

	proc->segments[S_STACK].base = stack;
	proc->segments[S_STACK].length = stack_size;
	proc->sp = stack_pointer;

	// Apply return values to the context on the stack (%d0 is at the top)
	*((uint32_t *) current_proc->sp) = proc->pid;
	*((uint32_t *) proc->sp) = 0;

	//dump((uint16_t *) current_proc->sp, 0x100);
	//dump((uint16_t *) proc->sp, 0x100);

	return 0;
}

int do_exit()
{
	free_proc(current_proc);
	return 0;
}

