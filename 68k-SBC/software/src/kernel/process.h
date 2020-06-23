
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
	uint16_t status;
	struct process *nextq;

	//struct mem_seg segments[NUM_SEGMENTS];
	struct mem_seg memory;
	void *sp;

	fd_table_t fd_table;
};

void init_proc();
struct process *new_proc(struct inode *inode);
void free_proc(struct process *proc);



static inline void save_state()
{
	asm volatile(
	"move.l	%d0, -(%sp)\n"
	"move.l	%d1, -(%sp)\n"
	"move.l	%d2, -(%sp)\n"
	"move.l	%d3, -(%sp)\n"
	"move.l	%d4, -(%sp)\n"
	"move.l	%d5, -(%sp)\n"
	"move.l	%d6, -(%sp)\n"
	"move.l	%d7, -(%sp)\n"
	"move.l	%a0, -(%sp)\n"
	"move.l	%a1, -(%sp)\n"
	"move.l	%a2, -(%sp)\n"
	"move.l	%a3, -(%sp)\n"
	"move.l	%a4, -(%sp)\n"
	"move.l	%a5, -(%sp)\n"
	"move.l	%a6, -(%sp)\n"
	);
}

static inline void swap_stack(void *old_sp, void *new_sp)
{
	asm volatile(
	"move.l	%%sp, (%0)\n"
	"move.l	(%1), %%sp\n"
	: // No Outputs
	: "a" (old_sp), "a" (new_sp)
	);
}

static inline void load_state()
{
	asm volatile(
	"move.l	(%sp)+, %a6\n"
	"move.l	(%sp)+, %a5\n"
	"move.l	(%sp)+, %a4\n"
	"move.l	(%sp)+, %a3\n"
	"move.l	(%sp)+, %a2\n"
	"move.l	(%sp)+, %a1\n"
	"move.l	(%sp)+, %a0\n"
	"move.l	(%sp)+, %d7\n"
	"move.l	(%sp)+, %d6\n"
	"move.l	(%sp)+, %d5\n"
	"move.l	(%sp)+, %d4\n"
	"move.l	(%sp)+, %d3\n"
	"move.l	(%sp)+, %d2\n"
	"move.l	(%sp)+, %d1\n"
	"move.l	(%sp)+, %d0\n"
	);
}



#endif
