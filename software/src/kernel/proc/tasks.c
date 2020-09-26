
#include <stddef.h>
#include <kernel/kmalloc.h>
#include <kernel/printk.h>

#include "../api.h"
#include "../interrupts.h"

#include "process.h"
#include "memory.h"

extern struct process *current_proc;

struct process *create_init_task()
{
	int error = 0;

	struct process *proc = new_proc(SU_UID);
	if (!proc)
		panic("Ran out of procs\n");

	// Setup memory segments
	int stack_size = 0x2000;
	proc->map.segments[M_TEXT].base = NULL;
	proc->map.segments[M_TEXT].length = 0x10000;
	proc->map.segments[M_STACK].base = kmalloc(stack_size);
	proc->map.segments[M_STACK].length = stack_size;

	extern void init_task();
	char *argv[1] = { NULL }, *envp[1] = { NULL };
	reset_stack(proc, init_task, argv, envp);

	return proc;
}

struct process *create_kernel_task(int (*task_start)())
{
	int error = 0;

	struct process *proc = new_proc(SU_UID);
	if (!proc)
		panic("Ran out of procs\n");

	int stack_size = 0x800;
	char *stack = kmalloc(stack_size);
	char *stack_pointer = stack + stack_size;

 	stack_pointer = create_context(stack_pointer, task_start);

	proc->map.segments[M_TEXT].base = NULL;
	proc->map.segments[M_TEXT].length = 0x10000;
	proc->map.segments[M_STACK].base = stack;
	proc->map.segments[M_STACK].length = stack_size;
	proc->sp = stack_pointer;

	return proc;
}

int idle_task()
{
	while (1) { }
}


