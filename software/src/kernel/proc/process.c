
#include <stddef.h>

#include <kernel/printk.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>

#include "../api.h"
#include "../interrupts.h"
#include "../misc/queue.h"

#include "timer.h"
#include "memory.h"
#include "process.h"
#include "filedesc.h"


// Process Table and Queues
#define PROCESS_MAX	6
static pid_t next_pid;
static struct process table[PROCESS_MAX];

extern struct process *current_proc;


void init_proc()
{
	next_pid = 2;

	for (short i = 0; i < PROCESS_MAX; i++) {
		table[i].pid = 0;
	}
}

struct process *new_proc(pid_t pid, uid_t uid)
{
	short saved_status;

	if (!pid)
		pid = next_pid++;

	for (short i = 0; i < PROCESS_MAX; i++) {
		if (!table[i].pid) {
			LOCK(saved_status);

			_queue_node_init(&table[i].node);
			table[i].pid = pid;
			if (current_proc) {
				table[i].parent = current_proc->pid;
				table[i].pgid = current_proc->pgid;
				table[i].session = current_proc->session;
				table[i].ctty = current_proc->ctty;
			}
			else {
				table[i].parent = INIT_PID;
				table[i].pgid = table[i].pid;
				table[i].session = table[i].pid;
				table[i].ctty = 0;
			}

			table[i].state = PS_RUNNING;
			table[i].sp = NULL;
			// Clear memory records
			for (char j = 0; j < NUM_SEGMENTS; j++) {
				table[i].map.segments[j].base = NULL;
				table[i].map.segments[j].length = 0;
			}

			table[i].bits = 0;
			table[i].exitcode = 0;
			table[i].wait_events = 0;
			table[i].wait_check = NULL;
			init_timer(&table[i].timer);
			init_signal_data(&table[i]);

			table[i].start_time = get_system_time();
			table[i].user_time = 0;
			table[i].sys_time = 0;

			table[i].uid = uid;
			for (char j = 0; j < PROC_CMDLINE_ARGS; j++)
				table[i].cmdline[j] = NULL;
			table[i].cwd = NULL;
			table[i].umask = PROC_DEFAULT_UMASK;
			init_fd_table(table[i].fd_table);

			insert_proc(&table[i]);

			UNLOCK(saved_status);
			return &table[i];
		}
	}
	return NULL;
}

struct process *get_proc(pid_t pid)
{
	for (short i = 0; i < PROCESS_MAX; i++) {
		if (table[i].pid == pid)
			return &table[i];
	}
	return NULL;
}

void close_proc(struct process *proc)
{
	remove_timer(&proc->timer);
	release_fd_table(proc->fd_table);
	free_process_memory(proc);

	// Reassign any child procs' parent to be 1 (init), since we can't be sure this proc's parent is waiting, and the zombie proc wont get recycled
	for (short i = 0; i < PROCESS_MAX; i++) {
		if (table[i].pid && table[i].parent == proc->pid)
			table[i].parent = 1;
	}
}


void cleanup_proc(struct process *proc)
{
	proc->pid = 0;
}

struct process *find_exited_child(pid_t parent, pid_t child)
{
	for (short i = 0; i < PROCESS_MAX; i++) {
		if (table[i].pid && table[i].state == PS_EXITED && table[i].parent == parent && (child == -1 || table[i].pid == child)) {
			return &table[i];
		}
	}
	return NULL;
}

static int on_alarm(struct timer *timer, struct process *proc)
{
	proc->bits &= ~PB_ALARM_ON;
	send_signal(proc->pid, SIGALRM);
}

int set_proc_alarm(struct process *proc, uint32_t seconds)
{
	if (!seconds)
		remove_timer(&proc->timer);
	else {
		proc->bits |= PB_ALARM_ON;
		proc->timer.argp = proc;
		proc->timer.callback = (timer_callback_t) on_alarm;
		add_timer(&proc->timer, seconds, 0);
	}
	return 0;
}


void proc_iter_start(struct process_iter *iter)
{
	iter->slot = 0;
}

struct process *proc_iter_next(struct process_iter *iter)
{
	struct process *proc;

	do {
		if (iter->slot >= PROCESS_MAX)
			return NULL;
		proc = &table[iter->slot++];
	} while (proc->pid == 0);
	return proc;
}

