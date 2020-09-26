
#include <string.h>
#include <sys/stat.h>
#include <kernel/vfs.h>

#include "mallocfs.h"
#include "alloc.h"

#define MAX_VNODES	20

extern struct vnode_ops mallocfs_vnode_ops;
static struct mallocfs_vnode vnode_table[MAX_VNODES];

void init_mallocfs_alloc()
{
	for (short i = 0; i < MAX_VNODES; i++) {
		vnode_table[i].vn.refcount = 0;
	}
}

struct vnode *new_mallocfs_vnode(struct mount *mp, device_t rdev, mode_t mode, uid_t uid)
{
	for (char i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].vn.refcount <= 0) {
			// TODO add in proper uid/gid, mtime, etc arguments
			vfs_init_vnode((struct vnode *) &vnode_table[i], &mallocfs_vnode_ops, mp, mode, 1, uid, 0, 0, rdev, 0, 0, 0);

			// NOTE we increment the refcount to represent the file link, otherwise the vnode will be freed when each file is not open
			vfs_clone_vnode(&vnode_table[i].vn);

			for (char j = 0; j < MALLOCFS_TOTAL_ZONES; j++)
				MALLOCFS_DATA(&vnode_table[i]).zones[j] = NULL;

			return (struct vnode *) &vnode_table[i];
		}
	}
	return NULL;
}

struct mallocfs_block *new_mallocfs_block(char zero)
{
	struct mallocfs_block *block;

	block = MFS_ALLOC_BLOCK();
	if (!block)
		return NULL;

	if (zero)
		memset_s(block, 0, MALLOCFS_BLOCK_SIZE);

	return block;
}

