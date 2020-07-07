
#include <stddef.h>
#include <kernel/syscall.h>


void exit(int exitcode)
{
	SYSCALL1(SYS_EXIT, exitcode);
	__builtin_unreachable();
}


