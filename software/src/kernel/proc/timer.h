
#ifndef _SRC_KERNEL_PROC_TIMER_H
#define _SRC_KERNEL_PROC_TIMER_H

#include <stdint.h>

#include "../misc/queue.h"

struct timer;

typedef int (*timer_callback_t)(struct timer *timer, void *argp);

struct timer {
	struct queue_node node;
	unsigned int expires_sec;
	unsigned int expires_usec;
	void *argp;
	timer_callback_t callback;
};

int init_timer_list();
void init_timer(struct timer *timer);
int add_timer(struct timer *timer, int seconds, int microseconds);
int remove_timer(struct timer *timer);
void check_timers();

#endif
