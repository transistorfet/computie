
#ifndef _SIGNAL_H
#define _SIGNAL_H

#define	SIGHUP		1	// Hangup
#define	SIGINT		2	// Interrupt
#define	SIGQUIT		3	// Quit
#define	SIGILL		4	// Illegal instruction
#define	SIGTRAP		5	// Trap
#define	SIGABRT		6	// Abort
#define SIGBUS		7	// Bus error
#define	SIGFPE		8	// Floating point exception
#define	SIGKILL		9	// Killed
#define SIGUSR1		10	// User Signal
#define	SIGSEGV		11	// Segmentation fault
#define	SIGUSR2		12	// User Signal
#define	SIGPIPE		13	// Broken pipe
#define	SIGALRM		14	// Alarm
#define	SIGTERM		15	// Termination
#define	SIGCHLD		17	// Child process terminated or stopped
#define	SIGCONT		18	// Continue
#define	SIGSTOP		19	// Stop, unblockable
#define	SIGTSTP		20	// Keyboard stop
#define	SIGTTIN		21	// Background read from control terminal
#define	SIGTTOU		22	// Background write to control terminal
#define	SIGXCPU		24	// CPU time limit exceeded
#define	SIGXFSZ		25	// File size limit exceeded
#define	SIGVTALRM	26	// Virtual timer expired
#define	SIGPROF		27	// Profiling timer expired
#define	SIGSYS		31	// Invalid system call

typedef unsigned int sigset_t;
typedef void (*sighandler_t)(int);

#define SIG_DFL		((sighandler_t) 0)
#define SIG_IGN		((sighandler_t) 1)

struct sigaction {
	sighandler_t sa_handler;	// SIG_DFL, SIG_IGN, or pointer to function
	sigset_t sa_mask;		// signals to be blocked during handler
	int sa_flags;			// special flags
};

// fields for sa_flags
#define SA_ONSTACK	0x0001	// deliver signal on alternate stack
#define SA_RESETHAND	0x0002	// reset signal handler when signal caught
#define SA_NODEFER	0x0004	// don't block signal while catching it
#define SA_RESTART	0x0008	// automatic system call restart
#define SA_SIGINFO	0x0010	// extended signal handling
#define SA_NOCLDWAIT	0x0020	// don't create zombies
#define SA_NOCLDSTOP	0x0040	// don't receive SIGCHLD when child stops

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

#endif

