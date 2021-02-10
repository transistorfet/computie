
#include <unistd.h>
#include <kernel/syscall.h>


int chown(const char *path, uid_t owner, gid_t group)
{
	return SYSCALL3(SYS_CHOWN, (int) path, (int) owner, (int) group);
}

