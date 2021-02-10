
#include <unistd.h>
#include <kernel/syscall.h>


void exit(int status)
{
	SYSCALL1(SYS_EXIT, status);
	__builtin_unreachable();
}

