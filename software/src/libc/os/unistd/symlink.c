
#include <sys/types.h>
#include <kernel/syscall.h>

int symlink(const char *target, const char *linkpath)
{
	return SYSCALL2(SYS_SYMLINK, (int) target, (int) linkpath);
}


