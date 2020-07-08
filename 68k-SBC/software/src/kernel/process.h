
#ifndef _SRC_KERNEL_PROCESS_H
#define _SRC_KERNEL_PROCESS_H

#include <stddef.h>
#include <kernel/filedesc.h>
#include <kernel/syscall.h>

#include "queue.h"

#define	NUM_SEGMENTS	2

typedef enum {
	S_TEXT,
	//S_DATA,
	S_STACK
} proc_seg_t;

struct mem_seg {
	void *base;
	size_t length;
};

typedef enum {
	PS_READY,
	PS_BLOCKED,
	PS_WAITING,
	PS_RESUMING,
	PS_EXITED,
} proc_state_t;


struct process {
	struct queue_node node;
	int pid;
	// parent
	//struct process *nextq;

	uint16_t state;		// proc_state_t
	int exitcode;
	// syscall restart info

	struct mem_seg segments[NUM_SEGMENTS];
	void *sp;

	struct syscall_record blocked_call;
	fd_table_t fd_table;
};

void init_proc();
struct process *new_proc();
void free_proc(struct process *proc);
void suspend_current_proc();
void resume_proc(struct process *proc);
void schedule();
struct process *create_kernel_task(int (*task_start)());
extern void *create_context(void *user_stack, void *entry);

#endif
