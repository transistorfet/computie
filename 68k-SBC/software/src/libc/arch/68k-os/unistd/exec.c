
#include <stddef.h>
#include <kernel/syscall.h>


int exec(const char *path)
{
	return SYSCALL2(SYS_EXEC, path, 0);
}


