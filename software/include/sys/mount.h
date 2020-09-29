
#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

struct mount_opts {
	const char *type;
	int mountflags;
	void *data;
};

int mount(const char *source, const char *target, struct mount_opts *opts);
int umount(const char *source);

#endif
