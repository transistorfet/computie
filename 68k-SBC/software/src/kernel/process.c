
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

struct process *new_proc(struct inode *inode)
{
	for (char i = 0; i < PROCESS_MAX; i++) {
		if (!table[i].pid) {
			table[i].pid = next_pid++;
			table[i].status = 0;
			table[i].nextq = NULL;

			init_fd_table(table[i].fd_table);

			// TODO use the inode to load an executable into memory?

			table[i].memory.base = NULL;
			table[i].memory.length = 0;
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

			if (proc->memory.base)
				free(proc->memory.base);

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


	if (!next || current_proc == next)
		return;		// TODO queue is empty...

	printf("next sp: %x\n", next->sp);

	// TODO just doing an equal time to each process

	//TRACE_ON();
	// TODO context switch from current_proc to next
	//context_switch(&current_proc->sp, &next->sp);

	current_proc->sp = current_proc_stack;
	current_proc = next;
	current_proc_stack = next->sp;

	//printf("prev sp: %x\n", current_proc->sp);
	//printf("next sp: %x\n", next->sp);

}


void context_switch(void *old_sp, void *new_sp)
{
	asm volatile(
	"move.l	%%d0, -(%%sp)\n"
	"move.l	%%d1, -(%%sp)\n"
	"move.l	%%d2, -(%%sp)\n"
	"move.l	%%d3, -(%%sp)\n"
	"move.l	%%d4, -(%%sp)\n"
	"move.l	%%d5, -(%%sp)\n"
	"move.l	%%d6, -(%%sp)\n"
	"move.l	%%d7, -(%%sp)\n"
	"move.l	%%a0, -(%%sp)\n"
	"move.l	%%a1, -(%%sp)\n"
	"move.l	%%a2, -(%%sp)\n"
	"move.l	%%a3, -(%%sp)\n"
	"move.l	%%a4, -(%%sp)\n"
	"move.l	%%a5, -(%%sp)\n"
	"move.l	%%a6, -(%%sp)\n"

	"move.l	%%sp, (%0)\n"
	"move.l	(%1), %%sp\n"

	"move.l	(%%sp)+, %%a6\n"
	"move.l	(%%sp)+, %%a5\n"
	"move.l	(%%sp)+, %%a4\n"
	"move.l	(%%sp)+, %%a3\n"
	"move.l	(%%sp)+, %%a2\n"
	"move.l	(%%sp)+, %%a1\n"
	"move.l	(%%sp)+, %%a0\n"
	"move.l	(%%sp)+, %%d7\n"
	"move.l	(%%sp)+, %%d6\n"
	"move.l	(%%sp)+, %%d5\n"
	"move.l	(%%sp)+, %%d4\n"
	"move.l	(%%sp)+, %%d3\n"
	"move.l	(%%sp)+, %%d2\n"
	"move.l	(%%sp)+, %%d1\n"
	"move.l	(%%sp)+, %%d0\n"

	"or.w	#0x8000, %%sr\n"
	"and.w	#0xF8FF, %%sr\n"
	: // No Outputs
	: "a" (old_sp), "a" (new_sp)
	);
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

			if (current_proc == proc)
				current_proc = NULL; // TODO you kinda need to switch to it properly.  If you make this NULL, it'll start from the head of the queue though
			break;
		}
	}
}
