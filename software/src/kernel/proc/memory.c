
#include <stddef.h>
#include <string.h>
#include <kernel/kmalloc.h>

#include "process.h"

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

char *copy_exec_args(char *stack, char *const argv[], char *const envp[])
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

	return stack;
}

int reset_stack(struct process *proc, void *entry, char *const argv[], char *const envp[])
{
	// Reset the stack to start our new process
	char *task_stack_pointer = proc->map.segments[M_STACK].base + proc->map.segments[M_STACK].length;

	// Setup new stack image
 	task_stack_pointer = copy_exec_args(task_stack_pointer, argv, envp);
 	task_stack_pointer = create_context(task_stack_pointer, entry, _exit);
	proc->sp = task_stack_pointer;

	return 0;
}

int clone_stack(struct process *parent_proc, struct process *proc)
{
	int stack_size = parent_proc->map.segments[M_STACK].length;
	char *stack = kmalloc(stack_size);
	char *stack_pointer = (stack + stack_size) - ((parent_proc->map.segments[M_STACK].base + parent_proc->map.segments[M_STACK].length) - parent_proc->sp);

	memcpy_s(stack, parent_proc->map.segments[M_STACK].base, parent_proc->map.segments[M_STACK].length);

	//printk("Parent Stack Pointer: %x\n", parent_proc->sp);
	//printk("Fork Stack: %x\n", stack);
	//printk("Fork Stack Top: %x\n", stack + stack_size);
	//printk("Fork Stack Pointer: %x\n", stack_pointer);

	proc->map.segments[M_STACK].base = stack;
	proc->map.segments[M_STACK].length = stack_size;
	proc->sp = stack_pointer;

	return 0;
}
