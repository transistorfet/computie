
#include <stdio.h>	// TODO for debugging only

#include <stdlib.h>

#include <kernel/signal.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>

#include "../api.h"
#include "../filedesc.h"
#include "../interrupts.h"
#include "../misc/queue.h"

#include "process.h"


// Info for Current Running Process (accessed by syscall interface)
void *kernel_stack;
void *current_proc_stack;
struct process *idle_proc;
struct process *current_proc;
struct syscall_record *current_syscall;

// TODO could this be used to calculate the offset of process_sp in syscall_entry??
//const process_sp_offset = offsetof(struct process, sp);
const int process_sp_offset = (int) &(((struct process *) NULL)->sp);

// Process Table and Queues
#define PROCESS_MAX	6
static pid_t next_pid;
static struct queue run_queue;
static struct queue blocked_queue;
static struct process table[PROCESS_MAX];


void init_proc()
{
	next_pid = 1;
	current_proc = NULL;

	_queue_init(&run_queue);
	_queue_init(&blocked_queue);

	for (short i = 0; i < PROCESS_MAX; i++) {
		table[i].pid = 0;
	}

	idle_proc = create_kernel_task(idle_task);
	_queue_remove(&run_queue, &idle_proc->node);
}

struct process *new_proc(uid_t uid)
{
	for (short i = 0; i < PROCESS_MAX; i++) {
		if (!table[i].pid) {
			_queue_node_init(&table[i].node);
			table[i].pid = next_pid++;
			if (current_proc) {
				table[i].parent = current_proc->pid;
				table[i].pgid = current_proc->pgid;
				table[i].session = current_proc->session;
				table[i].ctty = current_proc->ctty;
			}
			else {
				table[i].parent = 1;
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
			table[i].cwd = NULL;
			init_fd_table(table[i].fd_table);

			// Insert as runnable
			_queue_insert(&run_queue, &table[i].node);

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

void exit_proc(struct process *proc, int status)
{
	if (proc->state == PS_EXITED)
		return;
	else if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	else
		_queue_remove(&run_queue, &proc->node);

	proc->state = PS_EXITED;
	proc->exitcode = status;

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
		if (table[i].pid && table[i].state == PS_EXITED && table[i].parent == parent && (child == -1 || table[i].pid == child))
			return &table[i];
	}
	return NULL;
}

void set_proc_return_value(struct process *proc, int ret)
{
	*((uint32_t *) proc->sp) = ret;
}

void backup_current_proc()
{
	// Save the current process's stack pointer back to it's struct
	current_proc->sp = current_proc_stack;
}

void return_to_current_proc(int ret)
{
	current_proc_stack = current_proc->sp;
	if (current_proc->state == PS_RUNNING) {
		// If the process is still in the ready state, then set the return value in the process's context
		*((uint32_t *) current_proc_stack) = ret;
	}
	else {
		// If the process has been suspended, we wont return as normal but instead schedule another process
		schedule();
	}
}


void stop_proc(struct process *proc)
{
	if (proc->state == PS_EXITED || proc->state == PS_STOPPED)
		return;
	else if (proc->state == PS_RUNNING || proc->state == PS_RESUMING)
		_queue_remove(&run_queue, &proc->node);
	else if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	proc->state = PS_STOPPED;
}

void suspend_proc(struct process *proc, int flags)
{
	if (proc->state != PS_RUNNING)
		return;

	//proc->blocked_call.syscall = 0;
	//current_proc->blocked_call = *current_syscall;

	_queue_remove(&run_queue, &proc->node);
	_queue_insert(&blocked_queue, &proc->node);
	proc->state = PS_BLOCKED;
	proc->bits |= (flags & 0x0F);
}

void suspend_current_proc()
{
	suspend_proc(current_proc, PB_SYSCALL);
	current_proc->blocked_call = *current_syscall;
}


void resume_proc(struct process *proc)
{
	if (proc->state == PS_RUNNING || proc->state == PS_RESUMING || proc->state == PS_EXITED)
		return;
	if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	_queue_insert(&run_queue, &proc->node);
	proc->state = PS_RESUMING;
}

void resume_blocked_procs(int syscall_num, struct vnode *vnode, device_t rdev)
{
	struct process *cur = (struct process *) blocked_queue.tail;

	for (; cur; cur = (struct process *) cur->node.prev) {
		if (cur->state == PS_BLOCKED && cur->blocked_call.syscall == syscall_num) {
			int fd = cur->blocked_call.arg1;
			if ((vnode && cur->fd_table[fd]->vnode == vnode) || (rdev && cur->fd_table[fd]->vnode->rdev == rdev)) {
				resume_proc(cur);
			}
		}
	}
}

void resume_waiting_parent(struct process *proc)
{
	struct process *parent;

	parent = get_proc(proc->parent);
	if (parent->state == PS_BLOCKED && (parent->blocked_call.syscall == SYS_WAIT || parent->blocked_call.syscall == SYS_WAITPID))
		resume_proc(parent);
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
	time_t t = get_system_time();

	for (short i = 0; i < PROCESS_MAX; i++) {
		if (table[i].pid && (table[i].bits & PB_ALARM_ON) && t >= table[i].next_alarm) {
			table[i].bits &= ~PB_ALARM_ON;
			send_signal(table[i].pid, SIGALRM);
		}
	}
}


void schedule()
{
	struct process *next;

	if (!current_proc || !PROC_IS_RUNNABLE(current_proc) || !current_proc->node.next)
		next = (struct process *) run_queue.head;
	else
		next = (struct process *) current_proc->node.next;

	// No processes to run, so run the idle process
	if (!next)
		next = idle_proc;

	// Run the same process that was already running, so do nothing
	if (current_proc == next)
		return;

	current_proc->sp = current_proc_stack;
	current_proc = next;
	current_proc_stack = next->sp;

	//printk("RUN: %d\n", current_proc->pid);

	if (current_proc->state == PS_RESUMING) {
		//printk("Restarting %d with call %d\n", current_proc->pid, current_proc->blocked_call.syscall);
		current_proc->state = PS_RUNNING;
		if (current_proc->bits & PB_SYSCALL) {
			current_proc->bits &= ~PB_SYSCALL;
			current_syscall = &current_proc->blocked_call;
			do_syscall();
		}
	}
}

__attribute__((noreturn)) void begin_multitasking(struct process *proc)
{
	// Run the given process first
	current_proc = proc;
	current_proc_stack = current_proc->sp;

	//panic("Panicking for good measure\n");

	// Force an address error for testing
	//volatile uint16_t *data = (uint16_t *) 0x100001;
	//volatile uint16_t value = *data;

	// Start Multitasking
	asm("bra restore_context\n");
	__builtin_unreachable();
}

