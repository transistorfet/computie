
#ifndef _SRC_KERNEL_FS_MINIX_MKFS_H
#define _SRC_KERNEL_FS_MINIX_MKFS_H

#include "dir.h"

static int minix_mkfs(device_t dev)
{
	struct buf *super_buf;
	struct minix_v1_superblock *super_v1;
	struct minix_v1_superblock super_v1_cached;

	super_v1 = &super_v1_cached;

	super_v1->num_inodes = 0x40;
	super_v1->num_zones = 0x64;
	super_v1->imap_blocks = 1;
	super_v1->zmap_blocks = 1;
	super_v1->first_zone = 6;
	super_v1->log_zone_size = MINIX_V1_LOG_ZONE_SIZE;
	super_v1->max_file_size;	//?????
	super_v1->magic = 0x137F;
	super_v1->state = 0x0001;

	// Write the superblock
	super_buf = get_block(dev, MINIX_V1_SUPER_ZONE);
	if (!super_buf)
		return -1;
	super_v1 = (struct minix_v1_superblock *) super_buf->block;

	// TODO we assume this is not a disk yet, so manually initialize it
	memset_s(super_buf->block, 0x00, MINIX_V1_ZONE_SIZE);
	super_v1->num_inodes = to_le16(super_v1_cached.num_inodes);
	super_v1->num_zones = to_le16(super_v1_cached.num_zones);
	super_v1->imap_blocks = to_le16(super_v1_cached.imap_blocks);
	super_v1->zmap_blocks = to_le16(super_v1_cached.zmap_blocks);
	super_v1->first_zone = to_le16(super_v1_cached.first_zone);
	super_v1->log_zone_size = to_le16(super_v1_cached.log_zone_size);
	super_v1->max_file_size = to_le16(super_v1_cached.max_file_size);
	super_v1->magic = to_le16(super_v1_cached.magic);
	super_v1->state = to_le16(super_v1_cached.state);

	release_block(super_buf, BCF_DIRTY);

	super_v1 = &super_v1_cached;

	// Initialize bitmap zones
	bitmap_init(dev, MINIX_V1_INODE_BITMAP_START(super_v1), super_v1->imap_blocks, super_v1->num_inodes, 2);
	bitmap_init(dev, MINIX_V1_ZONE_BITMAP_START(super_v1), super_v1->zmap_blocks, super_v1->num_zones, 2);

	// Zero the inode table
	for (int i = 0; i < (super_v1->num_inodes >> MINIX_V1_LOG_INODES_PER_ZONE); i++) {
		struct buf *inode_buf = get_block(dev, MINIX_V1_INODE_TABLE_START(super_v1) + i);
		if (!inode_buf)
			return ENOMEM;
		memset_s(inode_buf->block, 0x00, MINIX_V1_ZONE_SIZE);
		release_block(inode_buf, BCF_DIRTY);
	}

	inode_t root_ino = 1;
	zone_t dir_zone = super_v1->first_zone;

	// Initialize root inode
	struct buf *inode_buf = get_block(dev, MINIX_V1_INODE_TABLE_START(super_v1));
	if (!inode_buf)
		return ENOMEM;
	struct minix_v1_inode *inode_table = inode_buf->block;

	inode_table[0].mode = to_le16(S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO);
	inode_table[0].uid = to_le16(SU_UID);
	inode_table[0].gid = 0;
	inode_table[0].size = to_le32(0);
	inode_table[0].nlinks = to_le16(1);
	inode_table[0].zones[0] = to_le16(dir_zone);
	release_block(inode_buf, BCF_DIRTY);

	// Initialize root directory
	struct buf *dir_buf = get_block(dev, dir_zone);
	if (!dir_buf)
		return ENOMEM;
	struct minix_v1_dirent *entries = dir_buf->block;
	memset_s(dir_buf->block, 0, MINIX_V1_ZONE_SIZE);

	entries[0].inode = to_le16((minix_v1_inode_t) root_ino);
	strcpy(entries[0].filename, ".");
	entries[1].inode = to_le16((minix_v1_inode_t) root_ino);
	strcpy(entries[1].filename, "..");

	release_block(dir_buf, BCF_DIRTY);

	//sync_bufcache();

	return 0;
}

#endif
