
#include <stdio.h>	// TODO for debugging only

#include <stdlib.h>

#include <kernel/filedesc.h>

#include "process.h"
#include "interrupts.h"
#include "queue.h"

void schedule();


void *kernel_stack;
void *current_proc_stack;
struct process *current_proc;

#define PROCESS_MAX	6
static int next_pid;
static struct queue run_queue;
static struct queue blocked_queue;
static struct process table[PROCESS_MAX];


void init_proc()
{
	next_pid = 1;
	current_proc = NULL;

	_queue_init(&run_queue);
	_queue_init(&blocked_queue);

	for (char i = 0; i < PROCESS_MAX; i++) {
		table[i].pid = 0;
	}
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
	release_fd_table(proc->fd_table);

	for (char j = 0; j < NUM_SEGMENTS; j++) {
		if (proc->segments[j].base)
			free(proc->segments[j].base);
	}

	_queue_remove(&run_queue, &proc->node);

	if (current_proc == proc) {
		current_proc = NULL;
		schedule();
	}
}

void ready_proc(struct process *proc)
{
	if (proc->state != PS_BLOCKED)
		return;
	_queue_remove(&blocked_queue, &proc->node);
	_queue_insert(&run_queue, &proc->node);
	proc->state = PS_READY;
}

void unready_proc(struct process *proc)
{
	if (proc->state != PS_READY)
		return;
	_queue_remove(&run_queue, &proc->node);
	_queue_insert(&blocked_queue, &proc->node);
	proc->state = PS_BLOCKED;
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

	putchar('!');

	if (!current_proc || !current_proc->node.next)
		next = (struct process *) run_queue.head;
	else
		next = (struct process *) current_proc->node.next;

	if (!next) {
		panic("No processes left to run... Halting\n");
		return;
	}

	//printf("next sp: %x\n", next->sp);

	if (current_proc == next)
		return;

	current_proc->sp = current_proc_stack;
	current_proc = next;
	current_proc_stack = next->sp;
}

/*
static inline void _queue_insert(struct process *proc)
{
	proc->nextq = run_queue;
	run_queue = proc;
}

static inline void _queue_remove(struct process *proc)
{
	struct process *prev = NULL;
	struct process *cur = run_queue;

	for (; cur; prev = cur, cur = cur->nextq) {
		if (cur == proc) {
			if (prev)
				prev->nextq = cur->nextq;
			else
				run_queue = cur->nextq;

			if (current_proc == proc) {
				current_proc = NULL;
				schedule();
			}
			break;
		}
	}
}
*/
