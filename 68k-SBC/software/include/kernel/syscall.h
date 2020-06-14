
#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H


#define SYS_GETCHAR	0
#define SYS_PUTCHAR	1
#define SYS_READ	2
#define SYS_WRITE	3


inline int SYSCALL1(int n, int a1)
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

	// HACK somehow this works. Being a separate block, the clobber list is empty, so it can chose to
	//	use %d0 as "ret" and thus no instructions are inserted and %d0 is returned
	//	Using an output operand above generates unnecessary move instructions
	//	I have a feeling this could break if the compiler decides to use another register
	asm("" : "=g" (ret));

	return ret;
}

inline int SYSCALL3(int n, int a1, void *a2, void *a3)
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

	// HACK somehow this works. Being a separate block, the clobber list is empty, so it can chose to
	//	use %d0 as "ret" and thus no instructions are inserted and %d0 is returned
	//	Using an output operand above generates unnecessary move instructions
	//	I have a feeling this could break if the compiler decides to use another register
	asm("" : "=g" (ret));

	return ret;
}

#endif
