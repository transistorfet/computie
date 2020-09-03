
#ifndef _SRC_KERNEL_FS_MALLOCFS_MALLOCFS_H
#define _SRC_KERNEL_FS_MALLOCFS_MALLOCFS_H

#include <kernel/vfs.h>

#define MALLOCFS_BLOCK_SIZE		1024
#define MALLOCFS_LOG_BLOCK_SIZE		__builtin_ctz(MALLOCFS_BLOCK_SIZE)

#define MALLOCFS_TIER1_ZONES		7
#define MALLOCFS_INDIRECT_TIERS		3
#define MALLOCFS_TOTAL_ZONES		MALLOCFS_TIER1_ZONES + MALLOCFS_INDIRECT_TIERS
#define MALLOCFS_MAX_FILENAME		12

#define MALLOCFS_DIRENTS		(MALLOCFS_BLOCK_SIZE / sizeof(struct mallocfs_dirent))
#define MALLOCFS_LOG_DIRENTS		__builtin_ctz(MALLOCFS_DIRENTS)
#define MALLOCFS_BLOCK_ZONES		(MALLOCFS_BLOCK_SIZE / sizeof(struct mallocfs_block *))
#define MALLOCFS_LOG_BLOCK_ZONES	__builtin_ctz(MALLOCFS_BLOCK_ZONES)

#define MALLOCFS_BLOCK(block)		((struct mallocfs_block *) (block))
#define MALLOCFS_DATA(vnode)		(((struct mallocfs_vnode *) (vnode))->data)

typedef unsigned int mallocfs_zone_t;

struct mallocfs_dirent {
	struct vnode *vnode;
	char name[MALLOCFS_MAX_FILENAME];
};

struct mallocfs_block {
	union {
		struct mallocfs_dirent entries[MALLOCFS_DIRENTS];
		struct mallocfs_block *zones[MALLOCFS_BLOCK_ZONES];
		char *data[MALLOCFS_BLOCK_SIZE];
	};
};

struct mallocfs_data {
	short links;
	device_t device;
	union {
		struct mallocfs_block *block;
		struct mallocfs_block *zones[MALLOCFS_TOTAL_ZONES];
	};
};

struct mallocfs_vnode {
	struct vnode vn;
	struct mallocfs_data data;
};

extern struct vnode *mallocfs_root;

int mallocfs_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result);
int mallocfs_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, struct vnode **result);
int mallocfs_lookup(struct vnode *vnode, const char *name, struct vnode **result);
int mallocfs_unlink(struct vnode *parent, struct vnode *vnode);
int mallocfs_truncate(struct vnode *vnode);
int mallocfs_release(struct vnode *vnode);

int mallocfs_open(struct vfile *file, int flags);
int mallocfs_close(struct vfile *file);
int mallocfs_read(struct vfile *file, char *buf, size_t nbytes);
int mallocfs_write(struct vfile *file, const char *buf, size_t nbytes);
int mallocfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t mallocfs_seek(struct vfile *file, offset_t position, int whence);
int mallocfs_readdir(struct vfile *file, struct vdir *dir);

#endif
