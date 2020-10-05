
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

typedef void (*sighandler_t)(int);

#define SIG_DFL		((sighandler_t) 0)
#define SIG_IGN		((sighandler_t) 1)

#endif

