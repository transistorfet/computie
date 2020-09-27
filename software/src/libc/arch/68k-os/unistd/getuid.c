
#include <kernel/vfs.h>
#include <kernel/syscall.h>


uid_t getuid()
{
	return SYSCALL1(SYS_GETUID, 0);
}

