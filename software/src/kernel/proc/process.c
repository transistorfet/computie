
#include <stddef.h>

#include <kernel/signal.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>

#include "../api.h"
#include "../filedesc.h"
#include "../interrupts.h"
#include "../misc/queue.h"

#include "process.h"


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
			table[i].next_alarm = 0;
			init_signal_data(&table[i]);

			table[i].uid = uid;
			table[i].cmdline = "";
			table[i].cwd = NULL;
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
	release_fd_table(proc->fd_table);
	for (char j = 0; j < NUM_SEGMENTS; j++) {
		if (proc->map.segments[j].base)
			kmfree(proc->map.segments[j].base);
	}

	// Reassign any child procs' parent to be this proc's parent
	for (short i = 0; i < PROCESS_MAX; i++) {
		if (table[i].pid && table[i].parent == proc->pid)
			table[i].parent = proc->parent;
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


void set_proc_return_value(struct process *proc, int ret)
{
	*((uint32_t *) proc->sp) = ret;
}

void return_to_current_proc(int ret)
{
	if (current_proc->state == PS_RUNNING) {
		// If the process is still in the ready state, then set the return value in the process's context
		*((uint32_t *) current_proc->sp) = ret;
	}
	else {
		// If the process has been suspended, we wont return as normal but instead schedule another process
		schedule();
	}
}


int set_alarm(struct process *proc, uint32_t seconds)
{
	if (!seconds)
		proc->bits &= ~PB_ALARM_ON;
	else {
		proc->bits |= PB_ALARM_ON;
		proc->next_alarm = get_system_time() + seconds;
	}
	return 0;
}

void check_timers()
{
	struct process *proc;
	struct process_iter iter;
	time_t t = get_system_time();

	proc_iter_start(&iter);
	while ((proc = proc_iter_next(&iter))) {
		if (proc->pid && (proc->bits & PB_ALARM_ON) && t >= proc->next_alarm) {
			proc->bits &= ~PB_ALARM_ON;
			send_signal(proc->pid, SIGALRM);
		}
	}
}


