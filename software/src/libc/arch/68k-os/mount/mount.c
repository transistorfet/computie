
#include <sys/mount.h>
#include <kernel/syscall.h>


int mount(const char *source, const char *target, struct mount_opts *opts)
{
	return SYSCALL3(SYS_MOUNT, (int) source, (int) target, (int) opts);
}


