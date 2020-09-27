
#include <kernel/vfs.h>
#include <kernel/syscall.h>


pid_t getppid()
{
	return SYSCALL1(SYS_GETPPID, 0);
}

