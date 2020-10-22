
#ifndef _SRC_KERNEL_PROCESS_H
#define _SRC_KERNEL_PROCESS_H

#include <stddef.h>
#include <kernel/signal.h>
#include <kernel/syscall.h>

#include "../filedesc.h"
#include "../misc/queue.h"

#define INIT_PID	1

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

struct mem_map {
	struct mem_seg segments[NUM_SEGMENTS];
};


#define PB_ALARM_ON			0x0001
#define PB_SYSCALL			0x0002
#define PB_WAITING			0x0004
#define PB_PAUSED			0x0008

#define PROC_IS_RUNNING(proc)		((proc)->state == PS_RUNNING || (proc)->state == PS_RESUMING)

typedef enum {
	PS_RUNNING,
	PS_RESUMING,
	PS_BLOCKED,
	PS_STOPPED,
	PS_EXITED,
} proc_state_t;


struct vnode;

struct process {
	struct queue_node node;

	void *sp;
	uint16_t state;
	struct mem_map map;

	pid_t pid;
	pid_t parent;
	pid_t pgid;
	pid_t session;

	uint16_t bits;
	int exitcode;
	time_t next_alarm;
	struct syscall_record blocked_call;
	struct signal_data signals;

	uid_t uid;
	device_t ctty;
	struct vnode *cwd;
	fd_table_t fd_table;
};

struct process_iter {
	short slot;
};

void init_proc();
struct process *new_proc(pid_t pid, uid_t uid);
struct process *get_proc(pid_t pid);
void close_proc(struct process *proc);
void cleanup_proc(struct process *proc);
struct process *find_exited_child(pid_t parent, pid_t child);

void proc_iter_start(struct process_iter *iter);
struct process *proc_iter_next(struct process_iter *iter);

void set_proc_return_value(struct process *proc, int ret);
void return_to_current_proc(int ret);

int set_alarm(struct process *proc, uint32_t seconds);
void check_timers();

#endif
