
#ifndef _SRC_KERNEL_FS_DEVFS_DEVFS_H
#define _SRC_KERNEL_FS_DEVFS_DEVFS_H

#include <kernel/vfs.h>


#define DEVFS_MAX_FILENAME	14
#define DEVFS_DIRENT_MAX	8

#define DEVFS_DATA(vnode)	(((struct devfs_vnode *) (vnode))->data)

struct devfs_dirent {
	struct vnode *vnode;
	char name[DEVFS_MAX_FILENAME];
};

struct devfs_block {
	union {
		struct devfs_dirent entries[DEVFS_DIRENT_MAX];
	}
};

struct devfs_data {
	device_t device;
};

struct devfs_vnode {
	struct vnode vn;
	struct devfs_data data;
};

extern struct vnode *devfs_root;

int devfs_create(struct vnode *vnode, const char *filename, mode_t mode, uid_t uid, struct vnode **result);
int devfs_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, uid_t uid, struct vnode **result);
int devfs_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int devfs_release(struct vnode *vnode);

int devfs_open(struct vfile *file, int flags);
int devfs_close(struct vfile *file);
int devfs_read(struct vfile *file, char *buf, size_t nbytes);
int devfs_write(struct vfile *file, const char *buf, size_t nbytes);
int devfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t devfs_seek(struct vfile *file, offset_t position, int whence);

#endif
