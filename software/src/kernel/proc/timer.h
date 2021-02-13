
#ifndef _SRC_KERNEL_PROC_TIMER_H
#define _SRC_KERNEL_PROC_TIMER_H

#include <stdint.h>

struct process;

//struct timer {
//	struct queue_node node;
//	int expires;
//};

int set_alarm(struct process *proc, uint32_t seconds);
void check_timers();

#endif
