
#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H


#define SYS_TEST	0
#define SYS_EXIT	1
#define SYS_FORK	2
#define SYS_READ	3
#define SYS_WRITE	4
#define SYS_OPEN	5
#define SYS_CLOSE	6
#define SYS_WAITPID	7
#define SYS_CREAT	8
#define SYS_LINK	9
#define SYS_UNLINK	10
#define SYS_EXECVE	11
#define SYS_CHDIR	12
#define SYS_TIME	13
#define SYS_MKNOD	14
#define SYS_CHMOD	15
#define SYS_CHOWN	16
#define SYS_BRK		17
#define SYS_STAT	18
#define SYS_LSEEK	19
#define SYS_GETPID	20
#define SYS_MOUNT	21
#define SYS_UMOUNT	22
#define SYS_SETUID	23
#define SYS_GETUID	24
#define SYS_STIME	25
#define SYS_PTRACE	26
#define SYS_ALARM	27
#define SYS_FSTAT	28
#define SYS_PAUSE	29
#define SYS_SIGRETURN	30
#define SYS_SIGACTION	31
#define SYS_ACCESS	32
#define SYS_SYNC	33
#define SYS_KILL	34
#define SYS_RENAME	35
#define SYS_MKDIR	36
#define SYS_RMDIR	37
#define SYS_DUP2	38
#define SYS_PIPE	39
#define SYS_IOCTL	40
#define SYS_FCNTL	41
#define SYS_READDIR	42
#define SYS_GETPPID	43
#define SYS_SYMLINK	44
#define SYS_GETPGID	45
#define SYS_SETPGID	46
#define SYS_GETSID	47
#define SYS_SETSID	48
#define SYS_UMASK	49
#define SYS_SBRK	50
#define SYS_SELECT	51

#define SYS_SOCKET	52
#define SYS_SOCKETPAIR	53
#define SYS_CONNECT	54
#define SYS_BIND	55
#define SYS_LISTEN	56
#define SYS_ACCEPT	57
#define SYS_SHUTDOWN	58
#define SYS_SEND	69
#define SYS_SENDTO	60
#define SYS_SENDMSG	61
#define SYS_RECV	62
#define SYS_RECVFROM	63
#define SYS_RECVMSG	64

// TODO remove this after testing
#define SYS_EXECBUILTIN	65

typedef int (*syscall_t)(int, int, int);

static inline int SYSCALL1(int n, int a1)
{
	register int ret;

	asm volatile(
	"move.l	%0, %%d0\n"
	"move.l	%1, %%d1\n"
	"trap	#1\n"
	: // Output in %d0
	: "g" (n), "g" (a1)
	: "%d0", "%d1"
	);

	asm volatile("move.l	%%d0, %0\n" : "=g" (ret) : : "%d0");

	return ret;
}

static inline int SYSCALL2(int n, int a1, int a2)
{
	register int ret;

	asm volatile(
	"move.l	%0, %%d0\n"
	"move.l	%1, %%d1\n"
	"move.l	%2, %%a0\n"
	"trap	#1\n"
	: // Output in %d0
	: "g" (n), "g" (a1), "g" (a2)
	: "%d0", "%d1", "%a0"
	);

	asm volatile("move.l	%%d0, %0\n" : "=g" (ret) : : "%d0");

	return ret;
}



static inline int SYSCALL3(int n, int a1, int a2, int a3)
{
	register int ret;

	asm volatile(
	"move.l	%0, %%d0\n"
	"move.l	%1, %%d1\n"
	"move.l	%2, %%a0\n"
	"move.l	%3, %%a1\n"
	"trap	#1\n"
	: // Output in %d0
	: "g" (n), "g" (a1), "g" (a2), "g" (a3)
	: "%d0", "%d1", "%a0", "%a1"
	);

	asm volatile("move.l	%%d0, %0\n" : "=g" (ret) : : "%d0");

	return ret;
}


struct syscall_record {
	int arg1;
	int arg2;
	int arg3;
	int syscall;
};

static inline void SYSCALL_SAVE(struct syscall_record *r, int n, int a1, int a2, int a3)
{
	r->arg1 = a1;
	r->arg2 = a2;
	r->arg3 = a3;
	r->syscall = n;
}


#endif
