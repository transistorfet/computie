
#ifndef _SRC_KERNEL_FS_NOPFS_H
#define _SRC_KERNEL_FS_NOPFS_H

int nop_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result);
int nop_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, struct vnode **result);
int nop_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int nop_unlink(struct vnode *parent, struct vnode *vnode);
int nop_truncate(struct vnode *vnode);
int nop_release(struct vnode *vnode);

int nop_open(struct vfile *file, int flags);
int nop_close(struct vfile *file);
int nop_read(struct vfile *file, char *buf, size_t nbytes);
int nop_write(struct vfile *file, const char *buf, size_t nbytes);
int nop_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t nop_seek(struct vfile *file, offset_t position, int whence);
int nop_readdir(struct vfile *file, struct vdir *dir);

#endif
