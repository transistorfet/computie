
#ifndef _SRC_KERNEL_PROC_TASKS_H
#define _SRC_KERNEL_PROC_TASKS_H

struct process;

struct process *create_init_task();
struct process *create_kernel_task(const char *name, int (*task_start)());
int idle_task();

#endif
