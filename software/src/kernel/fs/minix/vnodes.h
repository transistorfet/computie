
#ifndef _SRC_KERNEL_FS_MINIX_VNODES_H
#define _SRC_KERNEL_FS_MINIX_VNODES_H

#include <string.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/kmalloc.h>

#include "minix.h"
#include "inodes.h"

#define MAX_VNODES	20

static struct queue cache;
extern struct vnode_ops minix_vnode_ops;

static void init_minix_vnodes()
{
	_queue_init(&cache);
}

void sync_vnodes()
{
	struct minix_vnode *vnode;

	for (struct queue_node *cur = cache.head; cur; cur = cur->next) {
		vnode = (struct minix_vnode *) (((char *) cur) - sizeof(struct vnode));
		if (vnode->vn.bits & VBF_DIRTY)
			write_inode(&vnode->vn, vnode->data.ino);
	}
}

static struct minix_vnode *load_vnode(struct mount *mp, inode_t ino)
{
	int error;
	struct buf *buf;
	struct minix_vnode *vnode;

	// TODO this should recycle a cached vnode if it doesn't have a reference and there are more than a certain number of vnodes already loaded
	//entry = _find_free_entry();
	vnode = kmalloc(sizeof(struct minix_vnode));
	if (!vnode)
		return NULL;

	//printk("L:%x;%d;%d\n", vnode, mp->dev, ino);
	_queue_insert(&cache, &vnode->data.node);

	vfs_init_vnode((struct vnode *) vnode, &minix_vnode_ops, mp, 0, 0, 0, 0, 0);
	MINIX_DATA(vnode).device = 0;
	MINIX_DATA(vnode).ino = ino;
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		MINIX_DATA(vnode).zones[j] = NULL;

	error = read_inode(vnode, ino);
	if (error) {
		// TODO set errno
		kmfree(vnode);
		return NULL;
	}

	return vnode;
}

struct minix_vnode *get_vnode(struct mount *mp, inode_t ino)
{
	struct minix_vnode *vnode;

	for (struct queue_node *cur = cache.head; cur; cur = cur->next) {
		vnode = (struct minix_vnode *) (((char *) cur) - sizeof(struct vnode));
		//printk("S:%x;%d;%d\n", vnode, vnode->vn.mp->dev, vnode->data.ino);
		if (vnode->vn.mp->dev == mp->dev && vnode->data.ino == ino) {
			//printk("G:%x;%d;%d\n", vnode, mp->dev, ino);
			vnode->vn.refcount++;

			// Move to the front of the cache list
			_queue_remove(&cache, &vnode->data.node);
			_queue_insert(&cache, &vnode->data.node);
			return vnode;
		}
	}

	return load_vnode(mp, ino);
}

static struct minix_vnode *alloc_vnode(struct mount *mp, mode_t mode, uid_t uid, gid_t gid, device_t device)
{
	struct minix_vnode *vnode;

	// TODO this should pass in the mp or superblock, but that would mean alloc_inode couldn't be used by the hacky mkfs function
	bitnum_t ino = alloc_inode(&MINIX_SUPER(mp->super)->super_v1, mp->dev, mode, uid, gid);
	if (!ino)
		return NULL;

	vnode = load_vnode(mp, ino);
	if (!vnode) {
		free_inode(&MINIX_SUPER(mp->super)->super_v1, mp->dev, ino);;
		return NULL;
	}
	MINIX_DATA(vnode).device = device;
	//printk("C:%x;%d\n", vnode, MINIX_DATA(vnode).ino);
	return vnode;
}

void mark_vnode_dirty(struct minix_vnode *vnode)
{
	vnode->vn.bits |= VBF_DIRTY;
}

static int free_vnode(struct minix_vnode *vnode)
{
	if (vnode->vn.bits & VBF_DIRTY)
		write_inode(vnode, vnode->data.ino);
	_queue_remove(&cache, &vnode->data.node);
	free_inode(&MINIX_SUPER(vnode->vn.mp->super)->super_v1, vnode->vn.mp->dev, vnode->data.ino);
	kmfree(vnode);
	return 0;
}


#endif

