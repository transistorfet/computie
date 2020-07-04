
#ifndef _SRC_KERNEL_FS_DEVFS_DEVFS_H
#define _SRC_KERNEL_FS_DEVFS_DEVFS_H

#include <kernel/vfs.h>

extern struct vnode *devfs_root;

int devfs_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int devfs_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, struct vnode **result);
int devfs_open(struct vnode *vnode, mode_t mode, struct vfile **file);

int devfs_close(struct vfile *file);
int devfs_read(struct vfile *file, char *buf, size_t nbytes);
int devfs_write(struct vfile *file, const char *buf, size_t nbytes);
int devfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t devfs_seek(struct vfile *file, offset_t position, int whence);

#endif
