
#ifndef _SRC_KERNEL_FS_MINIX_INODES_H
#define _SRC_KERNEL_FS_MINIX_INODES_H

#include <string.h>
#include <sys/stat.h>
#include <kernel/vfs.h>

#include "minix.h"

static inode_t alloc_inode(struct minix_super *super, mode_t mode, uid_t uid, gid_t gid)
{
	bitnum_t inode_num;
	struct buf *inode_buf;
	block_t inode_table_zone;
	struct minix_v1_inode *inode_table;

	inode_num = bit_alloc(super->dev, MINIX_V1_INODE_BITMAP_START(&super->super_v1), super->super_v1.imap_blocks, 0);
	if (!inode_num)
		return ENOSPC;

	zone_t offset = inode_num >> MINIX_V1_LOG_INODES_PER_ZONE;
	inode_buf = get_block(super->dev, MINIX_V1_INODE_TABLE_START(&super->super_v1) + offset);
	if (!inode_buf)
		return ENOMEM;

	inode_table = inode_buf->block;
	inode_table[inode_num].mode = mode;
	inode_table[inode_num].uid = uid;
	inode_table[inode_num].gid = gid;
	inode_table[inode_num].size = 0;
	inode_table[inode_num].nlinks = 1;
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		inode_table[inode_num].zones[j] = NULL;

	release_block(inode_buf, BCF_DIRTY);

	return inode_num;
}

static int free_inode(struct minix_super *super, inode_t ino)
{
	return bit_free(super->dev, MINIX_V1_INODE_BITMAP_START(&super->super_v1), ino);
}

static int read_inode(struct vnode *vnode, inode_t ino)
{
	struct buf *inode_buf;
	block_t inode_table_zone;
	struct minix_super *super;
	struct minix_v1_inode *inode_table;

	super = MINIX_SUPER(vnode->mp->super);
	zone_t offset = ino >> MINIX_V1_LOG_INODES_PER_ZONE;
	inode_buf = get_block(super->dev, MINIX_V1_INODE_TABLE_START(&super->super_v1) + offset);
	if (!inode_buf)
		return ENOMEM;

	inode_table = inode_buf->block;
	vnode->mode = inode_table[ino].mode;
	vnode->uid = inode_table[ino].uid;
	vnode->gid = inode_table[ino].gid;
	vnode->size = inode_table[ino].size;
	// TODO we probably need to store this in minix_vnode_data
	//inode_table[ino].nlinks = 1;
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		MINIX_DATA(vnode).zones[j] = inode_table[ino].zones[j];

	vnode->bits &= ~VBF_DIRTY;
	release_block(inode_buf, 0);
	return 0;
}

static int write_inode(struct vnode *vnode, inode_t ino)
{
	struct buf *inode_buf;
	block_t inode_table_zone;
	struct minix_super *super;
	struct minix_v1_inode *inode_table;

	super = MINIX_SUPER(vnode->mp->super);
	zone_t offset = ino >> MINIX_V1_LOG_INODES_PER_ZONE;
	inode_buf = get_block(super->dev, MINIX_V1_INODE_TABLE_START(&super->super_v1) + offset);
	if (!inode_buf)
		return ENOMEM;

	inode_table = inode_buf->block;
	inode_table[ino].mode = vnode->mode;
	inode_table[ino].uid = vnode->uid;
	inode_table[ino].gid = vnode->gid;
	inode_table[ino].size = vnode->size;
	// TODO we probably need to store this in minix_vnode_data
	inode_table[ino].nlinks = 1;
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		inode_table[ino].zones[j] = MINIX_DATA(vnode).zones[j];

	vnode->bits &= ~VBF_DIRTY;
	release_block(inode_buf, BCF_DIRTY);
	return 0;
}


#endif
