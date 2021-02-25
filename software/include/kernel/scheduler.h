
#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include <sys/types.h>

struct vnode;
struct process;
struct syscall_record;

typedef int (*wait_check_t)(struct process *proc, int events, struct vnode *vnode, device_t rdev);

extern struct process *current_proc;
extern struct syscall_record *current_syscall;

void init_scheduler();
void insert_proc(struct process *proc);
void exit_proc(struct process *proc, int status);
void stop_proc(struct process *proc);
void suspend_proc(struct process *proc, int flags);
void resume_proc(struct process *proc);
void resume_proc_without_restart(struct process *proc);

void resume_waiting_parent(struct process *proc);
void resume_blocked_procs(int events, struct vnode *vnode, device_t rdev);
void suspend_syscall(struct process *proc, int proc_flags, int events, wait_check_t wait_check, struct syscall_record *syscall);
void cancel_syscall(struct process *proc);
void suspend_current_syscall(int events);
void restart_current_syscall();

void set_proc_return_value(struct process *proc, int ret);
void return_to_current_proc(int ret);

void request_reschedule();
void reschedule_proc_to_now(struct process *proc);
__attribute__((noreturn)) void begin_multitasking();

#endif

