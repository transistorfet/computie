
#ifndef _SRC_KERNEL_FS_PROCFS_PROCFS_H
#define _SRC_KERNEL_FS_PROCFS_PROCFS_H

#include <kernel/vfs.h>

#define PROCFS_DATA(vnode)	(((struct procfs_vnode *) (vnode))->data)

#define PFN_ROOTDIR	0
#define PFN_PROCDIR	1
#define PFN_CMDLINE	2
#define PFN_STAT	3
#define PFN_STATM	4

struct procfs_dir_entry {
	short filenum;
	char *filename;
};

struct procfs_data {
	pid_t pid;
	short filenum;
};

struct procfs_vnode {
	struct vnode vn;
	struct procfs_data data;
};


int procfs_init();
int procfs_mount(struct mount *mp, device_t dev, struct vnode *parent);
int procfs_unmount(struct mount *mp);

int procfs_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int procfs_release(struct vnode *vnode);

int procfs_open(struct vfile *file, int flags);
int procfs_close(struct vfile *file);
int procfs_read(struct vfile *file, char *buf, size_t nbytes);
int procfs_write(struct vfile *file, const char *buf, size_t nbytes);
int procfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t procfs_seek(struct vfile *file, offset_t position, int whence);
int procfs_readdir(struct vfile *file, struct dirent *dir);

#endif
