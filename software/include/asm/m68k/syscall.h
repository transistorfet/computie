
#ifndef _ASM_M68K_SYSCALL_H
#define _ASM_M68K_SYSCALL_H

static inline int SYSCALL1(int n, int a1)
{
	register int ret;

	asm volatile(
	"move.l	%1, %%d0\n"
	"move.l	%2, %%d1\n"
	"trap	#1\n"
	"move.l %%d0, %0\n"
	: "=g" (errno)
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
	"move.l	%1, %%d0\n"
	"move.l	%2, %%d1\n"
	"move.l	%3, %%a0\n"
	"trap	#1\n"
	"move.l %%d0, %0\n"
	: "=g" (errno)
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
	"move.l	%1, %%d0\n"
	"move.l	%2, %%d1\n"
	"move.l	%3, %%a0\n"
	"move.l	%4, %%a1\n"
	"trap	#1\n"
	"move.l %%d0, %0\n"
	: "=g" (errno)
	: "g" (n), "g" (a1), "g" (a2), "g" (a3)
	: "%d0", "%d1", "%a0", "%a1"
	);

	asm volatile("move.l	%%d0, %0\n" : "=g" (ret) : : "%d0");

	return ret;
}

static inline int SYSCALL5(int n, int a1, int a2, int a3, int a4, int a5)
{
	register int ret;

	asm volatile(
	"move.l	%1, %%d0\n"
	"move.l	%2, %%d1\n"
	"move.l	%3, %%a0\n"
	"move.l	%4, %%a1\n"
	"move.l	%5, %%d2\n"
	"move.l	%6, %%d3\n"
	"trap	#1\n"
	"move.l %%d0, %0\n"
	: "=g" (errno)
	: "g" (n), "g" (a1), "g" (a2), "g" (a3), "g" (a4), "g" (a5)
	: "%d0", "%d1", "%a0", "%a1", "%d2", "%d3"
	);

	asm volatile("move.l	%%d0, %0\n" : "=g" (ret) : : "%d0");

	return ret;
}

#endif
