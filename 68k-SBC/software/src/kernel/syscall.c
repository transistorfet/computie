
#include <stdio.h>

#include <kernel/driver.h>
#include <kernel/filedesc.h>

#include "api.h"
#include "process.h"
#include "interrupts.h"

#define SYSCALL_MAX	4

void testfunc() { puts("This is only a test"); }

void *syscall_table[SYSCALL_MAX] = {
	getchar,
	putchar,
	do_read,
	do_write,
};

extern void handle_syscall();

void init_syscall()
{
	set_interrupt(IV_TRAP1, handle_syscall);
}

// TODO this is temporary until you have processes working correctly
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



