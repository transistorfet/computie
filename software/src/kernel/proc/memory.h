
#ifndef _SRC_KERNEL_PROC_MEMORY_H
#define _SRC_KERNEL_PROC_MEMORY_H

struct process;

int create_process_memory(struct process *proc, size_t text_size);
void free_process_memory(struct process *proc);
int clone_process_memory(struct process *parent_proc, struct process *proc);
int reset_stack(struct process *proc, void *entry, char *const argv[], char *const envp[]);

#endif
