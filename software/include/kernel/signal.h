
#ifndef _KERNEL_SIGNALS_H
#define _KERNEL_SIGNALS_H

#include <signal.h>
#include <sys/types.h>

#define SIG_HANDLERS_NUM	32

struct signal_data {
	sigset_t ignored;
	sigset_t blocked;
	sigset_t pending;
	struct sigaction actions[SIG_HANDLERS_NUM];
};

struct process;

void init_signal_data(struct process *proc);
int get_signal_action(struct process *proc, int signum, struct sigaction *act);
int set_signal_action(struct process *proc, int signum, struct sigaction *act);
int send_signal(pid_t pid, int signum);
int dispatch_signal(struct process *proc, int signum);
void cleanup_signal_handler(struct process *proc);

#endif
