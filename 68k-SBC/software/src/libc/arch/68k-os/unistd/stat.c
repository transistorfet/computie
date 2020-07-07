
#include <unistd.h>
#include <kernel/syscall.h>


int stat(const char *path, struct stat *statbuf)
{
	return SYSCALL2(SYS_EXEC, path, (int) statbuf);
}



