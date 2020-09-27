
#include <sys/types.h>
#include <kernel/syscall.h>


time_t time()
{
	return SYSCALL1(SYS_TIME, 0);
}

