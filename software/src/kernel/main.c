
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

#include <kernel/syscall.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/vfs.h>

#include "api.h"
#include "process.h"
#include "interrupts.h"


extern void sh_task();


extern void init_mallocfs();
extern void init_minix();


extern struct driver tty_68681_driver;
extern struct vnode_ops mallocfs_vnode_ops;

struct driver *drivers[] = {
	&tty_68681_driver,
	NULL	// Null Termination of Driver List
};

extern struct process *current_proc;
extern void *current_proc_stack;


struct process *run_sh()
{
	int error = 0;

	struct process *proc = new_proc(SU_UID);

	current_proc = proc;

	// Open stdin
	int fd = do_open("tty", O_RDONLY, 0);
	if (fd < 0) {
		printk("Error opening file tty %d\n", error);
		return NULL;
	}

	// Open stdout
	fd = do_open("tty", O_WRONLY, 0);
	if (fd < 0) {
		printk("Error opening file tty %d\n", error);
		return NULL;
	}

	// Open stderr
	fd = do_open("tty", O_WRONLY, 0);
	if (fd < 0) {
		printk("Error opening file tty %d\n", error);
		return NULL;
	}

	// Setup memory segments
	int stack_size = 0x2000;
	proc->map.segments[M_TEXT].base = NULL;
	proc->map.segments[M_TEXT].length = 0x10000;
	proc->map.segments[M_STACK].base = malloc(stack_size);
	proc->map.segments[M_STACK].length = stack_size;

	// Set up initial stack
	char *stack_p = proc->map.segments[M_STACK].base + stack_size;
 	stack_p = create_context(stack_p, sh_task);
	proc->sp = stack_p;

	return proc;
}


int main()
{
	DISABLE_INTS();

	init_heap((void *) 0x110000, 0xD0000);

	init_interrupts();
	init_syscall();
	init_proc();

	init_vfs();
	init_mallocfs();
	//init_minix();

	// Initialize drivers
	for (char i = 0; drivers[i]; i++) {
		drivers[i]->init();
	}

	//init_minix();


	// Create initial task
	current_proc = run_sh();
	current_proc_stack = current_proc->sp;

	//panic("Panicking for good measure\n");

	// Start Multitasking
	asm("bra restore_context\n");

	// Force an address error for testing
	//volatile uint16_t *data = (uint16_t *) 0x100001;
	//volatile uint16_t value = *data;
}

