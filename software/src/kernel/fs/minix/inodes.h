
#ifndef _SRC_KERNEL_FS_MINIX_INODES_H
#define _SRC_KERNEL_FS_MINIX_INODES_H

#include <string.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <asm/macros.h>

#include "minix.h"

static inode_t alloc_inode(struct minix_super *super, mode_t mode, uid_t uid, gid_t gid, device_t rdev)
{
	bitnum_t inode_num;
	struct buf *inode_buf;
	block_t inode_table_zone;
	struct minix_v1_inode *inode_table;

	inode_num = bit_alloc(super->dev, MINIX_V1_INODE_BITMAP_START(&super->super_v1), super->super_v1.imap_blocks, 0);
	if (!inode_num)
		return ENOSPC;

	short inode_offset = (inode_num - 1) & (MINIX_V1_INODES_PER_ZONE - 1);
	zone_t block_offset = (inode_num - 1) >> MINIX_V1_LOG_INODES_PER_ZONE;
	inode_buf = get_block(super->dev, MINIX_V1_INODE_TABLE_START(&super->super_v1) + block_offset);
	if (!inode_buf)
		return ENOMEM;

	inode_table = inode_buf->block;
	inode_table[inode_offset].mode = to_le16(mode);
	inode_table[inode_offset].uid = to_le16(uid);
	inode_table[inode_offset].size = 0;
	inode_table[inode_offset].gid = (uint8_t) gid;
	inode_table[inode_offset].nlinks = (uint8_t) 1;
	// TODO add a/m/c time stamps
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		inode_table[inode_offset].zones[j] = NULL;
	if (mode & S_IFCHR)
		inode_table[inode_offset].zones[0] = to_le16(rdev);

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

	short inode_offset = (ino - 1) & (MINIX_V1_INODES_PER_ZONE - 1);
	zone_t block_offset = (ino - 1) >> MINIX_V1_LOG_INODES_PER_ZONE;
	inode_buf = get_block(super->dev, MINIX_V1_INODE_TABLE_START(&super->super_v1) + block_offset);
	if (!inode_buf)
		return ENOMEM;

	inode_table = inode_buf->block;
	vnode->mode = from_le16(inode_table[inode_offset].mode);
	vnode->uid = from_le16(inode_table[inode_offset].uid);
	vnode->size = from_le32(inode_table[inode_offset].size);
	vnode->atime = 0;
	vnode->mtime = from_le32(inode_table[inode_offset].mtime);
	vnode->ctime = 0;
	vnode->gid = (gid_t) inode_table[inode_offset].gid;
	vnode->nlinks = (uint8_t) inode_table[inode_offset].nlinks;
	vnode->rdev = from_le16((inode_table[inode_offset].mode & S_IFCHR) ? inode_table[inode_offset].zones[0] : 0);
	// NOTE: the zone numbers are stored in little endian in the vnode to make zone lookups easier
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		MINIX_DATA(vnode).zones[j] = inode_table[inode_offset].zones[j];

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
	short inode_offset = (ino - 1) & (MINIX_V1_INODES_PER_ZONE - 1);
	zone_t block_offset = (ino - 1) >> MINIX_V1_LOG_INODES_PER_ZONE;
	inode_buf = get_block(super->dev, MINIX_V1_INODE_TABLE_START(&super->super_v1) + block_offset);
	if (!inode_buf)
		return ENOMEM;

	inode_table = inode_buf->block;
	inode_table[inode_offset].mode = to_le16(vnode->mode);
	inode_table[inode_offset].uid = to_le16(vnode->uid);
	inode_table[inode_offset].size = to_le32(vnode->size);
	inode_table[inode_offset].mtime = to_le32(vnode->mtime);
	inode_table[inode_offset].gid = (uint8_t) vnode->gid;
	inode_table[inode_offset].nlinks = (uint8_t) vnode->nlinks;
	if (vnode->mode & S_IFCHR)
		MINIX_DATA(vnode).zones[0] = to_le16(vnode->rdev);
	// NOTE: the zone numbers are stored in little endian in the vnode to make zone lookups easier
	for (char j = 0; j < MINIX_V1_INODE_ZONENUMS; j++)
		inode_table[inode_offset].zones[j] = MINIX_DATA(vnode).zones[j];

	vnode->bits &= ~VBF_DIRTY;
	release_block(inode_buf, BCF_DIRTY);
	return 0;
}


#endif
