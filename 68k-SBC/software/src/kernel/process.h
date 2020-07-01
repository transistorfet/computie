
#ifndef _SRC_KERNEL_PROCESS_H
#define _SRC_KERNEL_PROCESS_H

#include <stddef.h>
#include <kernel/filedesc.h>

#define	NUM_SEGMENTS	2

typedef enum {
	S_TEXT,
	//S_DATA,
	S_STACK
} proc_seg_t;

struct mem_seg {
	void *base;
	size_t length;
};

struct process {
	int pid;
	// parent
	struct process *nextq;

	uint16_t status;
	int exitcode;

	struct mem_seg segments[NUM_SEGMENTS];
	void *sp;

	fd_table_t fd_table;
};

void init_proc();
struct process *new_proc();
void free_proc(struct process *proc);

#endif
