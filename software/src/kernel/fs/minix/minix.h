
#ifndef _SRC_KERNEL_FS_MINIX_MINIX_H
#define _SRC_KERNEL_FS_MINIX_MINIX_H

#include <sys/types.h>
#include <kernel/vfs.h>

#include "minix-v1.h"
#include "../../misc/queue.h"

#define MINIX_BOOT_ZONE		0
#define MINIX_SUPER_ZONE	1
#define MINIX_BITMAP_ZONES	2

#define MINIX_SUPER(super)	((struct minix_super *) (super))
#define MINIX_BLOCK(block)	((struct minix_block *) (block))
#define MINIX_VNODE(vnode)	((struct minix_vnode *) (vnode))
#define MINIX_DATA(vnode)	(((struct minix_vnode *) (vnode))->data)

#define MBF_DIRTY		0x0001


struct minix_super {
	device_t dev;
	char max_filename;

	union {
		struct minix_v1_superblock super_v1;
	};
};

struct minix_vnode_data {
	struct queue_node node;
	device_t device;		// device file (not mount device)
	minix_v1_inode_t ino;
	minix_v1_zone_t zones[MINIX_V1_INODE_ZONENUMS];
};

struct minix_vnode {
	struct vnode vn;
	struct minix_vnode_data data;
};


int minix_mount(struct mount *mp, device_t dev, struct vnode *parent);
int minix_unmount(struct mount *mp);
int minix_sync(struct mount *mp);
int minix_load_superblock(device_t dev);

int minix_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result);
int minix_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, struct vnode **result);
int minix_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int minix_unlink(struct vnode *parent, struct vnode *vnode);
int minix_rename(struct vnode *vnode, struct vnode *oldparent, const char *oldname, struct vnode *newparent, const char *newname);
int minix_truncate(struct vnode *vnode);
int minix_update(struct vnode *vnode);
int minix_release(struct vnode *vnode);

int minix_open(struct vfile *file, int flags);
int minix_close(struct vfile *file);
int minix_read(struct vfile *file, char *buf, size_t nbytes);
int minix_write(struct vfile *file, const char *buf, size_t nbytes);
int minix_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t minix_seek(struct vfile *file, offset_t position, int whence);
int minix_readdir(struct vfile *file, struct dirent *dir);


#endif
