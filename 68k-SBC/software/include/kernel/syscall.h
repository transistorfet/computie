
#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H


#define SYS_TEST	0
#define SYS_EXIT	1
#define SYS_FORK	2
#define SYS_READ	3
#define SYS_WRITE	4
#define SYS_OPEN	5
#define SYS_CLOSE	6
#define SYS_READDIR	7
#define SYS_EXEC	8
//#define SYS_WAIT	7
//#define SYS_CREAT	8
//#define SYS_LINK	9
//#define SYS_UNLINK	10
//#define SYS_EXEC	11


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

static inline int SYSCALL2(int n, const char *a1, int a2)
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



static inline int SYSCALL3(int n, int a1, void *a2, void *a3)
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

#endif
