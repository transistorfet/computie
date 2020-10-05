
#include <errno.h>

#include <sys/types.h>
#include <kernel/signal.h>

#include "process.h"


#define SIG_MASK_DEFAULT_IGNORE		0x00010000
#define SIG_MASK_DEFAULT_TERMINATE	0x47807FFF
#define SIG_MASK_DEFAULT_STOP		0x003C0000


static inline sigmap_t signal_to_map(int signum);
static void sig_default_terminate(struct process *proc, int signum);

void init_signal_data(struct process *proc)
{
	proc->signals.ignored = 0;
	proc->signals.blocked = 0;
	proc->signals.pending = 0;
}

int send_signal(pid_t pid, int signum)
{
	sigmap_t sigmask;
	struct process *proc;

	proc = get_proc(pid);
	if (!proc)
		return ESRCH;

	if (signum <= 0 || signum >= 32)
		return EINVAL;
	sigmask = signal_to_map(signum);

	if (sigmask & proc->signals.ignored)
		return 0;

	//proc->signals.pending |= sigmask;

	//if (sigmask & proc->signals.blocked)
	//	return 0;

	// TODO this is temporary because we don't execute signals properly, but still need some default actions
	if (sigmask & SIG_MASK_DEFAULT_TERMINATE) {
		printk("terminate\n");
		sig_default_terminate(proc, signum);
	}
	else if (sigmask & SIG_MASK_DEFAULT_STOP) {
		printk("suspend\n");
		stop_proc(proc);
	}
	else if (signum == SIGCONT) {
		printk("resume\n");
		resume_proc(proc);
	}

	return 0;
}

static void sig_default_terminate(struct process *proc, int signum)
{
	exit_proc(proc, -1);
	resume_waiting_parent(proc);
}

static inline sigmap_t signal_to_map(int signum)
{
	return 0x00000001 << (signum - 1);
}


int set_alarm(pid_t pid, uint32_t seconds)
{

}

void check_timers()
{
	// For each proc
	//   if proc is alive, and alarm bit is set, and alarm timeout has been reached
	//	send signal
}

