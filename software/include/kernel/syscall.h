
#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H


#define SYS_TEST	0
#define SYS_EXIT	1
#define SYS_FORK	2
#define SYS_READ	3
#define SYS_WRITE	4
#define SYS_OPEN	5
#define SYS_CLOSE	6
#define SYS_WAIT	7
#define SYS_CREAT	8
//#define SYS_LINK	9
#define SYS_UNLINK	10
#define SYS_EXEC	11
#define SYS_STAT	12
#define SYS_FSTAT	13
#define SYS_LSEEK	14
#define SYS_WAITPID	15
#define SYS_PIPE	16
#define SYS_READDIR	17
#define SYS_MKDIR	18
#define SYS_CHDIR	19
#define SYS_CHMOD	20
#define SYS_CHOWN	21
#define SYS_ACCESS	22


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
