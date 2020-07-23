
#include <unistd.h>
#include <kernel/syscall.h>


int unlink(const char *path)
{
	return SYSCALL2(SYS_UNLINK, path, 0);
}

