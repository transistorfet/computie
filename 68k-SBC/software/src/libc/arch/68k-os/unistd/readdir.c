
#include <kernel/vfs.h>
#include <kernel/syscall.h>


int readdir(int fd, struct vdir *dir)
{
	return SYSCALL3(SYS_READDIR, fd, dir, (void *) 0);
}

