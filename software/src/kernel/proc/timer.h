
#ifndef _SRC_KERNEL_PROC_TIMER_H
#define _SRC_KERNEL_PROC_TIMER_H

#include <stdint.h>

struct process;

int set_alarm(struct process *proc, uint32_t seconds);
void check_timers();

#endif
