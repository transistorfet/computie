
#include <signal.h>
#include <kernel/syscall.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	return SYSCALL3(SYS_SIGACTION, (int) signum, (int) act, (int) oldact);
}

