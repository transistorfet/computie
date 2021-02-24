
#include <kernel/time.h>
#include <kernel/signal.h>

#include "../misc/queue.h"

#include "timer.h"


static struct queue timer_list;

int init_timer_list()
{
	_queue_init(&timer_list);
}

void init_timer(struct timer *timer)
{
	_queue_node_init(&timer->node);
	timer->expires_sec = 0;
	timer->expires_usec = 0;
}

int add_timer(struct timer *timer, int seconds, int microseconds)
{
	time_t uptime[2];

	if (timer->expires_sec || timer->expires_usec)
		_queue_remove(&timer_list, &timer->node);
	get_system_uptime(uptime);
	timer->expires_sec = uptime[0] + seconds;
	timer->expires_usec = uptime[1] + microseconds;
	_queue_insert(&timer_list, &timer->node);
	return 0;
}

int remove_timer(struct timer *timer)
{
	if (!timer->expires_sec && !timer->expires_usec)
		return -1;
	_queue_remove(&timer_list, &timer->node);
	timer->expires_sec = 0;
	timer->expires_usec = 0;
	return 0;
}

void check_timers()
{
	time_t uptime[2];
	struct timer *next = NULL;

	get_system_uptime(uptime);
	for (struct timer *cur = (struct timer *) _queue_head(&timer_list); cur; cur = next) {
		next = (struct timer *) _queue_next(&cur->node);
		if (uptime[0] > cur->expires_sec && uptime[1] > cur->expires_usec) {
			remove_timer(cur);
			cur->callback(cur, cur->argp);
		}
	}
}

