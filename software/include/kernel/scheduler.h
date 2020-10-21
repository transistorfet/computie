
#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include <sys/types.h>

struct vnode;
struct process;

extern struct process *current_proc;

void exit_proc(struct process *proc, int status);
void stop_proc(struct process *proc);
void suspend_proc(struct process *proc, int flags);
void suspend_current_proc();
void resume_proc(struct process *proc);
void resume_blocked_procs(int syscall_num, struct vnode *vnode, device_t rdev);
void resume_waiting_parent(struct process *proc);
void reschedule_proc_to_now(struct process *proc);
void restart_current_syscall();

void schedule();
__attribute__((noreturn)) void begin_multitasking();

#endif

