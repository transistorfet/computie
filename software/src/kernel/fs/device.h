
#ifndef _SRC_KERNEL_FS_DEVICE_H
#define _SRC_KERNEL_FS_DEVICE_H

#include <kernel/driver.h>

extern struct vfile_ops device_vfile_ops;

int device_ops_open(struct vfile *file, int flags);
int device_ops_close(struct vfile *file);
int device_ops_read(struct vfile *file, char *buf, size_t nbytes);
int device_ops_write(struct vfile *file, const char *buf, size_t nbytes);
int device_ops_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid);
offset_t device_ops_seek(struct vfile *file, offset_t position, int whence);

#endif
