
#include "timer.h"
#include "process.h"

#include <kernel/time.h>
#include <kernel/signal.h>

int set_alarm(struct process *proc, uint32_t seconds)
{
	if (!seconds)
		proc->bits &= ~PB_ALARM_ON;
	else {
		proc->bits |= PB_ALARM_ON;
		proc->next_alarm = get_system_time() + seconds;
	}
	return 0;
}

void check_timers()
{
	struct process *proc;
	struct process_iter iter;
	time_t t = get_system_time();

	proc_iter_start(&iter);
	while ((proc = proc_iter_next(&iter))) {
		if (proc->pid && (proc->bits & PB_ALARM_ON) && t >= proc->next_alarm) {
			proc->bits &= ~PB_ALARM_ON;
			send_signal(proc->pid, SIGALRM);
		}
	}
}


