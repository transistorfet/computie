
#include <kernel/vfs.h>
#include <kernel/syscall.h>


int readdir(int fd, struct dirent *dir)
{
	return SYSCALL3(SYS_READDIR, fd, (int) dir, 0);
}

