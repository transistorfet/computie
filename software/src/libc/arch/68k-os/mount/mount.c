
#include <sys/mount.h>
#include <kernel/syscall.h>


int mount(const char *source, const char *target, const char *fstype, int mountflags, void *data)
{
	volatile struct mount_opts opts;

	opts.fstype = fstype;
	opts.mountflags = mountflags;
	opts.data = data;
	return SYSCALL3(SYS_MOUNT, (int) source, (int) target, (int) &opts);
}


