
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

struct vnode *new_mallocfs_vnode(device_t dev, mode_t mode)
{
	for (char i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].vn.refcount <= 0) {
			// TODO add in proper uid/gid, mtime, etc arguments
			vfs_init_vnode((struct vnode *) &vnode_table[i], &mallocfs_vnode_ops, mode, 0, 0, 0, 0);

			MALLOCFS_DATA(&vnode_table[i]).links = 1;
			MALLOCFS_DATA(&vnode_table[i]).device = dev;
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

struct vnode *new_mallocfs_vnode_with_block(mode_t mode, struct vnode *parent)
{
	struct vnode *vnode;
	struct mallocfs_block *block;

	vnode = new_mallocfs_vnode(0, mode);
	if (!vnode)
		return NULL;

	block = new_mallocfs_block(MFS_ZERO);
	if (!block) {
		vfs_release_vnode(vnode);
		return NULL;
	}

	MALLOCFS_DATA(vnode).block = block;
	if (mode & S_IFDIR) {
		block->entries[0].vnode = vnode;
		strcpy(block->entries[0].name, ".");

		block->entries[1].vnode = parent;
		strcpy(block->entries[1].name, "..");
	}

	return vnode;
}


