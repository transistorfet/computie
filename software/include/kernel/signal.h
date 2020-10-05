
#ifndef _KERNEL_SIGNALS_H
#define _KERNEL_SIGNALS_H

#include <signal.h>
#include <sys/types.h>

typedef unsigned int sigmap_t;

struct signal_data {
	sigmap_t ignored;
	sigmap_t blocked;
	sigmap_t pending;
};

struct process;

void init_signal_data(struct process *proc);
int send_signal(pid_t pid, int signum);
int set_alarm(pid_t pid, uint32_t seconds);

#endif
