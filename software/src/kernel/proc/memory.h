
#ifndef _SRC_KERNEL_PROC_MEMORY_H
#define _SRC_KERNEL_PROC_MEMORY_H

#include "process.h"

char *copy_exec_args(char *stack, char *const argv[], char *const envp[]);
int reset_stack(struct process *proc, void *entry, char *const argv[], char *const envp[]);
int clone_stack(struct process *parent_proc, struct process *proc);


#endif
