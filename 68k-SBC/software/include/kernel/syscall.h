
#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H

#define SYS_PUTS	1
#define SYS_PRINTF	2
#define SYS_PUTCHAR	3

#define SYSCALL(n, a1, a2, a3) {	\
	asm(				\
	"move.l	%0, %%d0\n"		\
	"move.l	%1, %%d1\n"		\
	"move.l	%2, %%a0\n"		\
	"move.l	%3, %%a1\n"		\
	"trap	#1\n"			\
	:				\
	: "r" (n), "r" (a1), "r" (a2), "r" (a3)	\
	: "%d0", "%d1", "%a0", "%a1" 	\
	);				\
}

#endif
