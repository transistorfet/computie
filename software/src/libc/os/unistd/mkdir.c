
#include <unistd.h>
#include <kernel/syscall.h>


int mkdir(const char *path, mode_t mode)
{
	return SYSCALL2(SYS_MKDIR, (int) path, (int) mode);
}

