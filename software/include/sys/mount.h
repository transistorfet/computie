
#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#define MOUNT_RO	0x01
#define MOUNT_RW	0x00

struct mount_opts {
	const char *fstype;
	int mountflags;
	void *data;
};

int mount(const char *source, const char *target, const char *fstype, int mountflags, void *data);
int umount(const char *source);

#endif
