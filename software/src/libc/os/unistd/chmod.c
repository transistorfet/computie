
#include <unistd.h>
#include <kernel/syscall.h>


int chmod(const char *path, mode_t mode)
{
	return SYSCALL2(SYS_CHMOD, (int) path, (int) mode);
}

