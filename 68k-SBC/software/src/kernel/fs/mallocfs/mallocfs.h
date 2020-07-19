
#ifndef _SRC_KERNEL_FS_MALLOCFS_MALLOCFS_H
#define _SRC_KERNEL_FS_MALLOCFS_MALLOCFS_H

#include <kernel/vfs.h>

#define MALLOCFS_MAX_FILENAME	14
#define MALLOCFS_BLOCK_SIZE	2048
#define MALLOCFS_DIRENTS	(MALLOCFS_BLOCK_SIZE / sizeof(struct mallocfs_dirent))

#define MALLOCFS_BLOCK(block)	((struct mallocfs_block *) (block))

struct mallocfs_dirent {
	struct vnode *vnode;
	char name[MALLOCFS_MAX_FILENAME];
};

struct mallocfs_block {
	union {
		struct mallocfs_dirent entries[MALLOCFS_DIRENTS];
		char *data[MALLOCFS_BLOCK_SIZE];
	};
};

// TODO this will be needed when we allocate vnodes ourself but for ease we will put the dev and fsdata pointers into vnode
/*
struct mallocfs_node {
	struct vnode vnode;
	device_t dev;
	struct mallocfs_block *block;
};
*/

extern struct vnode *mallocfs_root;

int mallocfs_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result);
int mallocfs_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, struct vnode **result);
int mallocfs_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int mallocfs_unlink(struct vnode *parent, struct vnode *vnode);

int mallocfs_open(struct vfile *file, int flags);
int mallocfs_close(struct vfile *file);
int mallocfs_read(struct vfile *file, char *buf, size_t nbytes);
int mallocfs_write(struct vfile *file, const char *buf, size_t nbytes);
int mallocfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t mallocfs_seek(struct vfile *file, offset_t position, int whence);
int mallocfs_readdir(struct vfile *file, struct vdir *dir);

#endif
