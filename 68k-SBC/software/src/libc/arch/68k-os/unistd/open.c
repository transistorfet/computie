
#include <stddef.h>
#include <kernel/syscall.h>


int open(const char *path, int flags)
{
	return SYSCALL2(SYS_OPEN, path, flags);
}

