
#include <asm/macros.h>

#include <kernel/printk.h>
#include <kernel/scheduler.h>

#include "tasks.h"
#include "process.h"
#include "../api.h"
#include "../interrupts.h"
#include "../misc/queue.h"

// Info for Current Running Process (accessed by syscall interface)
void *kernel_stack;
int need_reschedule;
int kernel_reentries;
struct process *idle_proc;
struct process *current_proc;
struct syscall_record *current_syscall;

// TODO could this be used to calculate the offset of process_sp in syscall_entry??
//const process_sp_offset = offsetof(struct process, sp);
//const int process_sp_offset = (int) &(((struct process *) NULL)->sp);

static struct queue run_queue;
static struct queue blocked_queue;

void init_scheduler()
{
	need_reschedule = 0;
	kernel_reentries = 1;
	current_proc = NULL;

	_queue_init(&run_queue);
	_queue_init(&blocked_queue);

	create_init_task();

	idle_proc = create_kernel_task(idle_task);
	_queue_remove(&run_queue, &idle_proc->node);
}

void insert_proc(struct process *proc)
{
	_queue_insert(&run_queue, &proc->node);
}

void exit_proc(struct process *proc, int status)
{
	short saved_status;

	if (proc->state == PS_EXITED)
		return;

	LOCK(saved_status);
	if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	else
		_queue_remove(&run_queue, &proc->node);

	proc->state = PS_EXITED;
	proc->exitcode = status;

	close_proc(proc);
	need_reschedule = 1;
	UNLOCK(saved_status);
}

void stop_proc(struct process *proc)
{
	short saved_status;

	LOCK(saved_status);
	if (proc->state == PS_EXITED || proc->state == PS_STOPPED) {
		UNLOCK(saved_status);
		return;
	}
	else if (proc->state == PS_RUNNING || proc->state == PS_RESUMING)
		_queue_remove(&run_queue, &proc->node);
	else if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	proc->state = PS_STOPPED;
	need_reschedule = 1;
	UNLOCK(saved_status);
}

void suspend_proc(struct process *proc, int flags)
{
	short saved_status;

	LOCK(saved_status);
	if (proc->state != PS_RUNNING) {
		UNLOCK(saved_status);
		return;
	}

	_queue_remove(&run_queue, &proc->node);
	_queue_insert(&blocked_queue, &proc->node);
	proc->state = PS_BLOCKED;
	proc->bits |= (flags & 0x0F);
	need_reschedule = 1;
	UNLOCK(saved_status);
}

void suspend_current_proc()
{
	short saved_status;

	LOCK(saved_status);
	suspend_proc(current_proc, PB_SYSCALL);
	current_proc->blocked_call = *current_syscall;
	UNLOCK(saved_status);
}


void resume_proc(struct process *proc)
{
	short saved_status;

	LOCK(saved_status);
	if (proc->state == PS_RUNNING || proc->state == PS_RESUMING || proc->state == PS_EXITED) {
		UNLOCK(saved_status);
		return;
	}
	else if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	_queue_insert(&run_queue, &proc->node);
	proc->state = PS_RESUMING;
	UNLOCK(saved_status);
}

void resume_blocked_procs(int syscall_num, struct vnode *vnode, device_t rdev)
{
	short saved_status;
	struct process *cur;

	LOCK(saved_status);
	cur = (struct process *) blocked_queue.tail;
	for (; cur; cur = (struct process *) cur->node.prev) {
		if (cur->state == PS_BLOCKED && cur->blocked_call.syscall == syscall_num) {
			int fd = cur->blocked_call.arg1;
			if ((vnode && cur->fd_table[fd]->vnode == vnode) || (rdev && cur->fd_table[fd]->vnode->rdev == rdev)) {
				resume_proc(cur);
			}
		}
	}
	UNLOCK(saved_status);
}

void resume_waiting_parent(struct process *proc)
{
	struct process *parent;

	parent = get_proc(proc->parent);
	if (parent->state == PS_BLOCKED && (parent->bits & PB_WAITING))
		resume_proc(parent);
}

void reschedule_proc_to_now(struct process *proc)
{
	short saved_status;

	LOCK(saved_status);
	if (proc == current_proc || !PROC_IS_RUNNING(proc) || !PROC_IS_RUNNING(current_proc)) {
		UNLOCK(saved_status);
		return;
	}

	_queue_remove(&run_queue, &proc->node);
	_queue_insert_after(&run_queue, &proc->node, &current_proc->node);
	UNLOCK(saved_status);
}

void restart_current_syscall()
{
	short saved_status;

	if (current_proc->bits & PB_SYSCALL) {
		LOCK(saved_status);
		current_proc->bits &= ~PB_SYSCALL;
		current_syscall = &current_proc->blocked_call;
		UNLOCK(saved_status);

		do_syscall();
	}
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
}


void request_reschedule()
{
	need_reschedule = 1;
}

void schedule()
{
	short saved_status;
	struct process *next;

	LOCK(saved_status);
	need_reschedule = 0;
	if (!current_proc || !PROC_IS_RUNNING(current_proc) || !current_proc->node.next)
		next = (struct process *) run_queue.head;
	else
		next = (struct process *) current_proc->node.next;

	// No processes to run, so run the idle process
	if (!next)
		next = idle_proc;

	// Switch the current process
	current_proc = next;
	UNLOCK(saved_status);

	if (current_proc->state == PS_RESUMING) {
		current_proc->state = PS_RUNNING;
		restart_current_syscall();
	}
}

__attribute__((noreturn)) void begin_multitasking()
{
	current_proc = (struct process *) run_queue.head;

	//panic("Panicking for good measure\n");

	// Force an address error for testing
	//volatile uint16_t *data = (uint16_t *) 0x100001;
	//volatile uint16_t value = *data;

	// Start Multitasking
	GOTO_LABEL("restore_context");
	__builtin_unreachable();
}

