
#include <unistd.h>
#include <kernel/syscall.h>


int fstat(int fd, struct stat *statbuf)
{
	return SYSCALL2(SYS_FSTAT, (const char *) fd, (int) statbuf);
}

