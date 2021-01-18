
#ifndef _SRC_KERNEL_PROC_BINARIES_H
#define _SRC_KERNEL_PROC_BINARIES_H

struct process;

int load_binary(const char *path, struct process *proc, void **entry);

#endif
