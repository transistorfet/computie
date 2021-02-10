
#include <unistd.h>
#include <kernel/syscall.h>


int chdir(const char *path)
{
	return SYSCALL1(SYS_CHDIR, (int) path);
}

