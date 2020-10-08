
#include <errno.h>

#include <sys/types.h>
#include <kernel/signal.h>

#include "process.h"


#define SIG_MASK_DEFAULT_IGNORE		0x00010000
#define SIG_MASK_DEFAULT_TERMINATE	0x47807FFF
#define SIG_MASK_DEFAULT_STOP		0x003C0000


static inline void run_signal_handler(struct process *proc, int signum);
static inline void run_signal_default_action(struct process *proc, int signum, sigset_t sigmask);
static void sig_default_terminate(struct process *proc, int signum);
static inline sigset_t signal_to_map(int signum);

void init_signal_data(struct process *proc)
{
	proc->signals.ignored = 0;
	proc->signals.blocked = 0;
	proc->signals.pending = 0;

	for (short i = 0; i < SIG_HANDLERS_NUM; i++)
		proc->signals.actions[i].sa_handler = NULL;
}

int get_signal_action(struct process *proc, int signum, struct sigaction *act)
{
	*act = proc->signals.actions[signum - 1];
}

int set_signal_action(struct process *proc, int signum, struct sigaction *act)
{
	proc->signals.actions[signum - 1] = *act;
	return 0;
}

int send_signal(pid_t pid, int signum)
{
	struct process *proc;

	proc = get_proc(pid);
	if (!proc)
		return ESRCH;
	return dispatch_signal(proc, signum);
}

int send_signal_to_process_group(pid_t pgid, int signum)
{
	// TODO need to traverse procs...
}

int dispatch_signal(struct process *proc, int signum)
{
	sigset_t sigmask;

	if (signum <= 0 || signum >= 32)
		return EINVAL;
	sigmask = signal_to_map(signum);

	if (sigmask & proc->signals.ignored)
		return 0;

	if (sigmask & proc->signals.blocked) {
		proc->signals.pending |= sigmask;
		return 0;
	}

	if (proc->signals.actions[signum - 1].sa_handler)
		run_signal_handler(proc, signum);
	else
		run_signal_default_action(proc, signum, sigmask);

	return 0;
}

static inline void run_signal_handler(struct process *proc, int signum)
{
	// TODO use the action sa_mask value to block, and you'll need to push some extra data onto the stack for sigreturn to reverse the mask changes
	proc->sp = create_context(proc->sp, proc->signals.actions[signum - 1].handlers, _sigreturn);
	resume_proc(proc);
	// TODO this is a hack to skip over the resuming state
	proc->state = PS_RUNNING;
}

void cleanup_signal_handler(struct process *proc)
{
	proc->sp = drop_context(proc->sp);
	if ((proc->bits & PB_SYSCALL) && !(proc->bits & PB_PAUSED))
		//suspend_current_proc();
		//current_proc->state = PS_RESUMING;
		suspend_proc(proc, 0);
}

static inline void run_signal_default_action(struct process *proc, int signum, sigset_t sigmask)
{
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
}

static void sig_default_terminate(struct process *proc, int signum)
{
	exit_proc(proc, -1);
	resume_waiting_parent(proc);
}

static inline sigset_t signal_to_map(int signum)
{
	return 0x00000001 << (signum - 1);
}



