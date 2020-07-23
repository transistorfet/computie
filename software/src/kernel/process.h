
#ifndef _SRC_KERNEL_PROCESS_H
#define _SRC_KERNEL_PROCESS_H

#include <stddef.h>
#include <kernel/filedesc.h>
#include <kernel/syscall.h>

#include "queue.h"

#define	NUM_SEGMENTS	2

typedef enum {
	M_TEXT,
	//M_DATA,
	M_STACK
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
	int parent;
	uint16_t state;
	int exitcode;

	struct mem_seg segments[NUM_SEGMENTS];
	void *sp;

	struct syscall_record blocked_call;
	fd_table_t fd_table;
};

void init_proc();
struct process *new_proc();
struct process *get_proc(int pid);
void exit_proc(struct process *proc, int status);
void cleanup_proc(struct process *proc);
struct process *find_exited_child(int parent, int child);

void suspend_current_proc();
void resume_proc(struct process *proc);
void resume_blocked_procs(int syscall_num, struct vnode *vnode);
void resume_all_procs();

void schedule();
struct process *create_kernel_task(int (*task_start)());

extern void *create_context(void *user_stack, void *entry);

#endif
