
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <kernel/kmalloc.h>

#include "memory.h"
#include "process.h"
#include "context.h"

int copy_string_array(char **stack, int *count, char *const arr[])
{
	int len = 0;
	*count = 0;

	for (int i = 0; arr[i] != NULL; i++) {
		// String, line terminator, and a pointer 
		len += sizeof(char *) + strlen(arr[i]) + 1;
		*count += 1;
	}
	len += sizeof(char *);

	// Align to the nearest word
	if (len & 0x01)
		len += 1;

	char **dest_arr = (char **) (*stack - len);
	char *buffer = ((char *) dest_arr) + (sizeof(char *) * (*count + 1));
	*stack = (char *) dest_arr;

	int i = 0, j = 0;
	for (; j < *count; j++) {
		dest_arr[j] = &buffer[i];
		strcpy(dest_arr[j], arr[j]);
		i += strlen(arr[j]) + 1;
	}
	dest_arr[j] = NULL;

	return 0;
}

char *copy_exec_args(char *stack, char *const argv[], char *const envp[], const char **proc_args)
{
	int argc, envc;
	char **stack_argv, **stack_envp;

	copy_string_array(&stack, &envc, envp);
	stack_envp = (char **) stack;
	copy_string_array(&stack, &argc, argv);
	stack_argv = (char **) stack;

	stack -= sizeof(char **);
	*((char ***) stack) = stack_envp;
	stack -= sizeof(char **);
	*((char ***) stack) = stack_argv;
	stack -= sizeof(int);
	*((int *) stack) = argc;

	for (char j = 0; j < PROC_CMDLINE_ARGS; j++)
		proc_args[j] = j < argc ? stack_argv[j] : NULL;

	return stack;
}

int create_process_memory(struct process *proc, size_t text_size)
{
	char *text = kmalloc(text_size);

	// TODO overwriting this could be a memory leak if it's not already NULL.  How do I refcount segments?
	//if (proc->map.segments[M_TEXT].base)
	//	kmfree(proc->map.segments[M_TEXT].base);
	proc->map.segments[M_TEXT].base = text;
	proc->map.segments[M_TEXT].length = text_size;
	return 0;
}

void free_process_memory(struct process *proc)
{
	struct process *cur;
	struct process_iter iter;

	// If another process is sharing the same text segment, then set it to NULL so we don't free it
	proc_iter_start(&iter);
	while ((cur = proc_iter_next(&iter))) {
		if (cur != proc && cur->map.segments[M_TEXT].base == proc->map.segments[M_TEXT].base) {
			proc->map.segments[M_TEXT].base = NULL;
			break;
		}
	}

	if (proc->map.segments[M_TEXT].base)
		kmfree(proc->map.segments[M_TEXT].base);
	if (proc->map.segments[M_DATA].base)
		kmfree(proc->map.segments[M_DATA].base);

	proc->map.segments[M_TEXT].base = NULL;
	proc->map.segments[M_TEXT].length = 0;
	proc->map.segments[M_DATA].base = NULL;
	proc->map.segments[M_DATA].length = 0;
	proc->map.segments[M_STACK].base = NULL;
	proc->map.segments[M_STACK].length = 0;
	/*
	for (char j = 0; j < NUM_SEGMENTS; j++) {
		if (proc->map.segments[j].base)
			kmfree(proc->map.segments[j].base);
	}
	*/
}

int clone_process_memory(struct process *parent_proc, struct process *proc)
{
	int stack_size = parent_proc->map.segments[M_STACK].length;
	char *stack = kmalloc(stack_size);
	char *stack_pointer = (stack + stack_size) - ((parent_proc->map.segments[M_STACK].base + parent_proc->map.segments[M_STACK].length) - parent_proc->sp);

	memcpy_s(stack, parent_proc->map.segments[M_STACK].base, parent_proc->map.segments[M_STACK].length);

	proc->map.segments[M_DATA].base = stack;
	proc->map.segments[M_DATA].length = 0;
	proc->map.segments[M_STACK].base = proc->map.segments[M_DATA].base;
	proc->map.segments[M_STACK].length = stack_size;
	proc->sp = stack_pointer;

	proc->map.segments[M_TEXT].base = parent_proc->map.segments[M_TEXT].base;
	proc->map.segments[M_TEXT].length = parent_proc->map.segments[M_TEXT].length;

	// Copy the relevant process data from the parent to child
	proc->cwd = parent_proc->cwd;
	dup_fd_table(proc->fd_table, parent_proc->fd_table);

	for (char j = 0; j < PROC_CMDLINE_ARGS; j++) {
		if (parent_proc->cmdline[j])
			proc->cmdline[j] = proc->map.segments[M_STACK].base + (parent_proc->cmdline[j] - (char *) parent_proc->map.segments[M_STACK].base);
		else
			proc->cmdline[j] = NULL;
	}

	return 0;
}

int reset_stack(struct process *proc, void *entry, char *const argv[], char *const envp[])
{
	// TODO this might be wrong if the data segment contains non-heap memory areas
	// Reset the data segment to be 0
	proc->map.segments[M_STACK].length += proc->map.segments[M_DATA].length;
	proc->map.segments[M_STACK].base = proc->map.segments[M_DATA].base;
	proc->map.segments[M_DATA].length = 0;

	// Reset the stack to start our new process
	char *task_stack_pointer = proc->map.segments[M_STACK].base + proc->map.segments[M_STACK].length;

	// Setup new stack image
	task_stack_pointer = copy_exec_args(task_stack_pointer, argv, envp, proc->cmdline);
	task_stack_pointer = create_context(task_stack_pointer, entry, _exit);
	proc->sp = task_stack_pointer;

	return 0;
}

int increase_data_segment(struct process *proc, int increase)
{
	if (proc->map.segments[M_DATA].base + increase >= proc->sp)
		return ENOMEM;
	if (((ssize_t) proc->map.segments[M_DATA].length) + increase < 0)
		return ENOMEM;
	// Require an alignment to 4 bytes
	if ((increase > 0 ? increase : -increase) & 0x3)
		return ENOMEM;

	proc->map.segments[M_DATA].length += increase;
	proc->map.segments[M_STACK].base += increase;
	proc->map.segments[M_STACK].length -= increase;

	return 0;
}

