
#ifndef _SRC_KERNEL_FS_MINIX_VNODES_H
#define _SRC_KERNEL_FS_MINIX_VNODES_H

#include <string.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/kmalloc.h>

#include "minix.h"
#include "inodes.h"

#define MAX_VNODES	20

// The queue_node is not at the start of the vnode, so we need to adjust the address to point to the start of the vnode struct
#define MINIX_QUEUE_NODE_TO_VNODE(node) \
			((struct vnode *) (((char *) node) - sizeof(struct vnode)))

static int nodes_in_cache;
static struct queue cache;
extern struct vnode_ops minix_vnode_ops;

static void release_unused_vnodes();

static void init_minix_vnodes()
{
	nodes_in_cache = 0;
	_queue_init(&cache);
}

static void sync_vnodes()
{
	struct vnode *vnode;

	for (struct queue_node *cur = cache.head; cur; cur = cur->next) {
		vnode = MINIX_QUEUE_NODE_TO_VNODE(cur);
		if (vnode->bits & VBF_DIRTY)
			write_inode(vnode, vnode->ino);
	}
}

static struct vnode *load_vnode(struct mount *mp, inode_t ino)
{
	int error;
	struct buf *buf;
	struct vnode *vnode;

	release_unused_vnodes();

	// TODO this should recycle a cached vnode if it doesn't have a reference and there are more than a certain number of vnodes already loaded
	//entry = _find_free_entry();
	vnode = kmalloc(sizeof(struct minix_vnode));
	if (!vnode)
		return NULL;

	//printk("L:%x;%d;%d\n", vnode, mp->dev, ino);
	_queue_insert(&cache, &MINIX_DATA(vnode).node);

	vfs_init_vnode(vnode, &minix_vnode_ops, mp, 0, 1, 0, 0, 0, ino, 0, 0, 0, 0);
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		MINIX_DATA(vnode).zones[j] = NULL;

	error = read_inode(vnode, ino);
	if (error) {
		// TODO set errno
		kmfree(vnode);
		return NULL;
	}

	nodes_in_cache += 1;
	return vnode;
}

static struct vnode *get_vnode(struct mount *mp, inode_t ino)
{
	struct vnode *vnode;

	for (struct queue_node *cur = cache.head; cur; cur = cur->next) {
		vnode = MINIX_QUEUE_NODE_TO_VNODE(cur);
		//printk("S:%x;%d;%d\n", vnode, vnode->mp->dev, vnode->ino);
		if (vnode->mp->dev == mp->dev && vnode->ino == ino) {
			//printk("G:%x;%d;%d\n", vnode, mp->dev, ino);
			vfs_clone_vnode(vnode);

			// Move to the front of the cache list
			_queue_remove(&cache, &MINIX_DATA(vnode).node);
			_queue_insert(&cache, &MINIX_DATA(vnode).node);
			return vnode;
		}
	}

	return load_vnode(mp, ino);
}

static struct vnode *alloc_vnode(struct mount *mp, mode_t mode, uid_t uid, gid_t gid, device_t device)
{
	struct vnode *vnode;

	// TODO this should pass in the mp or superblock, but that would mean alloc_inode couldn't be used by the hacky mkfs function
	bitnum_t ino = alloc_inode(MINIX_SUPER(mp->super), mode, uid, gid, device);
	if (!ino)
		return NULL;

	vnode = load_vnode(mp, ino);
	if (!vnode) {
		free_inode(MINIX_SUPER(mp->super), ino);;
		return NULL;
	}

	//printk("C:%x;%d\n", vnode, vnode->ino);
	return vnode;
}

static void mark_vnode_dirty(struct vnode *vnode)
{
	vnode->bits |= VBF_DIRTY;
}

static int delete_vnode(struct vnode *vnode)
{
	free_inode(MINIX_SUPER(vnode->mp->super), vnode->ino);
	vnode->ino = 0;
	return 0;
}

static int release_vnode(struct vnode *vnode)
{
	if (vnode->ino && vnode->bits & VBF_DIRTY)
		write_inode(vnode, vnode->ino);
	_queue_remove(&cache, &MINIX_DATA(vnode).node);
	kmfree(vnode);
	nodes_in_cache -= 1;
	return 0;
}

static void release_unused_vnodes()
{
	struct queue_node *last;

	while (nodes_in_cache - 1 >= MAX_VNODES) {
		for (last = cache.tail; last && MINIX_QUEUE_NODE_TO_VNODE(last)->refcount > 0; last = last->prev) { }
		if (!last || MINIX_QUEUE_NODE_TO_VNODE(last)->refcount > 0)
			break;
		release_vnode(MINIX_QUEUE_NODE_TO_VNODE(last));
	}
}


#endif

