
#include <stdio.h>	// TODO for debugging only

#include <stdlib.h>

#include <kernel/filedesc.h>

#include "process.h"
#include "interrupts.h"

static inline void _queue_insert(struct process *proc);
static inline void _queue_remove(struct process *proc);

void *kernel_stack;
void *current_proc_stack;
struct process *current_proc;

#define PROCESS_MAX	6
static int next_pid;
static struct process *run_queue;
static struct process table[PROCESS_MAX];


void init_proc()
{
	next_pid = 1;
	run_queue = NULL;
	current_proc = NULL;

	for (char i = 0; i < PROCESS_MAX; i++) {
		table[i].pid = 0;
	}
}

struct process *new_proc()
{
	for (char i = 0; i < PROCESS_MAX; i++) {
		if (!table[i].pid) {
			table[i].pid = next_pid++;
			table[i].status = 0;
			table[i].nextq = NULL;

			init_fd_table(table[i].fd_table);

			// TODO use the inode to load an executable into memory?

			// Clear memory records
			for (char j = 0; j < NUM_SEGMENTS; j++) {
				table[i].segments[j].base = NULL;
				table[i].segments[j].length = 0;
			}
			table[i].sp = NULL;

			_queue_insert(&table[i]);

			return &table[i];
		}
	}
	return NULL;
}

void free_proc(struct process *proc)
{
	//for (char i = 0; i < PROCESS_MAX; i++) {
	//	if (table[i].pid == pid) {
			//table[i].pid = 0;
			proc->pid = 0;
			// TODO free all the things

			for (char j = 0; j < NUM_SEGMENTS; j++) {
				if (proc->segments[j].base)
					free(proc->segments[j].base);
			}

			_queue_remove(proc);

	//		break;
	//	}
	//}
}


void print_run_queue()
{
	for (struct process *cur = run_queue; cur; cur = cur->nextq) {
		printf("%d: sp = %x\n", cur->pid, cur->sp);
	}
}

void schedule()
{
	struct process *next;

	putchar('!');

	if (!current_proc || !current_proc->nextq)
		next = run_queue;
	else
		next = current_proc->nextq;

	if (!next) {
		panic("No processes left to run... Halting\n");
		return;
	}

	if (current_proc == next)
		return;

	printf("next sp: %x\n", next->sp);

	current_proc->sp = current_proc_stack;
	current_proc = next;
	current_proc_stack = next->sp;

	//printf("prev sp: %x\n", current_proc->sp);
	//printf("next sp: %x\n", next->sp);
}

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
