
#ifndef _SRC_KERNEL_PROCESS_H
#define _SRC_KERNEL_PROCESS_H

#include <stddef.h>
#include <kernel/filedesc.h>

#define	NUM_SEGMENTS	3

typedef enum {
	TEXT,
	DATA,
	STACK
} proc_seg_t;

struct mem_seg {
	void *base;
	size_t length;
};

struct process {
	int pid;
	struct mem_seg segments[NUM_SEGMENTS];
	fd_table_t fd_table;
};

void init_proc();
struct process *new_proc(struct inode *inode);
void free_proc(int pid);

#endif
