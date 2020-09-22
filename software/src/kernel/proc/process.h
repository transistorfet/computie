
#ifndef _SRC_KERNEL_PROCESS_H
#define _SRC_KERNEL_PROCESS_H

#include <stddef.h>
#include <kernel/syscall.h>

#include "../filedesc.h"
#include "../misc/queue.h"

#define	NUM_SEGMENTS			2

typedef enum {
	M_TEXT,
	//M_DATA,
	M_STACK
} proc_seg_t;

struct mem_seg {
	void *base;
	size_t length;
};

struct mem_map {
	struct mem_seg segments[NUM_SEGMENTS];
};

#define PROC_IS_RUNNABLE(proc)		((proc)->state == PS_READY || (proc)->state == PS_RESUMING)

typedef enum {
	PS_READY,
	PS_BLOCKED,
	PS_WAITING,
	PS_RESUMING,
	PS_EXITED,
} proc_state_t;


struct vnode;

struct process {
	struct queue_node node;
	pid_t pid;
	pid_t parent;
	uint16_t state;

	struct mem_map map;
	void *sp;

	int exitcode;
	struct syscall_record blocked_call;

	//struct process_fs_data {
	uid_t uid;
	struct vnode *cwd;
	//};
	fd_table_t fd_table;
};

void init_proc();
struct process *new_proc(uid_t uid);
struct process *get_proc(pid_t pid);
void exit_proc(struct process *proc, int status);
void cleanup_proc(struct process *proc);
struct process *find_exited_child(pid_t parent, pid_t child);

void set_proc_return(struct process *proc, int ret);
void backup_current_proc();
void return_to_current_proc(int ret);
void suspend_current_proc();
void resume_proc(struct process *proc);
void resume_blocked_procs(int syscall_num, struct vnode *vnode);

void schedule();
__attribute__((noreturn)) void begin_multitasking(struct process *proc);
struct process *create_kernel_task(int (*task_start)());

extern void *create_context(void *user_stack, void *entry);

#endif
