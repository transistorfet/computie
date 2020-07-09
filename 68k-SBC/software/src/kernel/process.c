
#include <stdio.h>	// TODO for debugging only

#include <stdlib.h>

#include <kernel/filedesc.h>

#include "process.h"
#include "interrupts.h"
#include "queue.h"


// Info for Current Running Process (accessed by syscall interface)
void *kernel_stack;
void *current_proc_stack;
struct process *idle_proc;
struct process *current_proc;
struct syscall_record *current_syscall;


// Process Table and Queues
#define PROCESS_MAX	6
static int next_pid;
static struct queue run_queue;
static struct queue blocked_queue;
static struct process table[PROCESS_MAX];

static int idle_task();


void init_proc()
{
	next_pid = 1;
	current_proc = NULL;

	_queue_init(&run_queue);
	_queue_init(&blocked_queue);

	for (char i = 0; i < PROCESS_MAX; i++) {
		table[i].pid = 0;
	}

	idle_proc = create_kernel_task(idle_task);
	_queue_remove(&run_queue, &idle_proc->node);
}

struct process *new_proc()
{
	for (char i = 0; i < PROCESS_MAX; i++) {
		if (!table[i].pid) {
			table[i].pid = next_pid++;
			table[i].state = PS_READY;
			table[i].node.next = NULL;
			table[i].node.prev = NULL;

			init_fd_table(table[i].fd_table);

			// TODO use the inode to load an executable into memory?

			// Clear memory records
			for (char j = 0; j < NUM_SEGMENTS; j++) {
				table[i].segments[j].base = NULL;
				table[i].segments[j].length = 0;
			}
			table[i].sp = NULL;

			_queue_insert(&run_queue, &table[i].node);

			return &table[i];
		}
	}
	return NULL;
}

void free_proc(struct process *proc)
{
	proc->pid = 0;
	proc->state = PS_EXITED;
	release_fd_table(proc->fd_table);

	for (char j = 0; j < NUM_SEGMENTS; j++) {
		if (proc->segments[j].base)
			free(proc->segments[j].base);
	}

	_queue_remove(&run_queue, &proc->node);
}


void suspend_current_proc()
{
	current_proc->blocked_call = *current_syscall;

	if (current_proc->state != PS_READY)
		return;
	_queue_remove(&run_queue, &current_proc->node);
	_queue_insert(&blocked_queue, &current_proc->node);
	current_proc->state = PS_BLOCKED;
}

void resume_proc(struct process *proc)
{
	if (proc->state != PS_BLOCKED)
		return;
	_queue_remove(&blocked_queue, &proc->node);
	_queue_insert(&run_queue, &proc->node);
	proc->state = PS_RESUMING;
}

void resume_all_procs()
{
	struct process *cur = (struct process *) blocked_queue.tail;

	for (; cur; cur = cur->node.prev)
		resume_proc(cur);
}




void print_run_queue()
{
	for (struct queue_node *cur = run_queue.head; cur; cur = cur->next) {
		struct process *proc = (struct process *) cur;
		printf("%d: sp = %x\n", proc->pid, proc->sp);
	}
}

void schedule()
{
	struct process *next;

	//putchar('!');

	if (!current_proc || !current_proc->node.next)
		next = (struct process *) run_queue.head;
	else
		next = (struct process *) current_proc->node.next;

	if (!next) {
		next = idle_proc;
		//panic("No processes left to run... Halting\n");
		//return;
	}

	//printf("next sp: %x\n", next->sp);

	if (current_proc == next)
		return;

	current_proc->sp = current_proc_stack;
	current_proc = next;
	current_proc_stack = next->sp;

	if (current_proc->state == PS_RESUMING) {
		current_proc->state = PS_READY;
		current_syscall = &current_proc->blocked_call;
		do_syscall();
	}
}

struct process *create_kernel_task(int (*task_start)())
{
	int error = 0;

	struct process *proc = new_proc();
	if (!proc)
		panic("Ran out of procs\n");

	int stack_size = 0x800;
	char *stack = malloc(stack_size);
	char *stack_pointer = stack + stack_size;

 	stack_pointer = create_context(stack_pointer, task_start);

	proc->segments[S_TEXT].base = NULL;
	proc->segments[S_TEXT].length = 0x10000;
	proc->segments[S_STACK].base = stack;
	proc->segments[S_STACK].length = stack_size;
	proc->sp = stack_pointer;

	return proc;
}

static int idle_task()
{
	while (1) { }
}


