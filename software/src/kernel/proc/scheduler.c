
#include <kernel/printk.h>
#include <kernel/scheduler.h>

#include "tasks.h"
#include "process.h"
#include "../api.h"
#include "../misc/queue.h"

// Info for Current Running Process (accessed by syscall interface)
void *kernel_stack;
void *current_proc_stack;
struct process *idle_proc;
struct process *current_proc;
struct syscall_record *current_syscall;

// TODO could this be used to calculate the offset of process_sp in syscall_entry??
//const process_sp_offset = offsetof(struct process, sp);
const int process_sp_offset = (int) &(((struct process *) NULL)->sp);

static struct queue run_queue;
static struct queue blocked_queue;

void init_scheduler()
{
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
	if (proc->state == PS_EXITED)
		return;
	else if (proc->state == PS_BLOCKED)
		_queue_remove(&blocked_queue, &proc->node);
	else
		_queue_remove(&run_queue, &proc->node);

	proc->state = PS_EXITED;
	proc->exitcode = status;

	close_proc(proc);
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
	if (parent->state == PS_BLOCKED && (parent->bits & PB_WAITING))
		resume_proc(parent);
}

void restart_current_syscall()
{
	if (current_proc->bits & PB_SYSCALL) {
		current_proc->bits &= ~PB_SYSCALL;
		current_syscall = &current_proc->blocked_call;
		do_syscall();
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

__attribute__((noreturn)) void begin_multitasking()
{
	// Run the given process first
	//current_proc = proc;
	//current_proc_stack = current_proc->sp;
	current_proc = (struct process *) run_queue.head;
	current_proc_stack = current_proc->sp;

	//panic("Panicking for good measure\n");

	// Force an address error for testing
	//volatile uint16_t *data = (uint16_t *) 0x100001;
	//volatile uint16_t value = *data;

	// Start Multitasking
	asm("bra restore_context\n");
	__builtin_unreachable();
}

