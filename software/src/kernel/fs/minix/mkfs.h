
#ifndef _SRC_KERNEL_FS_MINIX_MKFS_H
#define _SRC_KERNEL_FS_MINIX_MKFS_H

#include "dir.h"

static int minix_mkfs(device_t dev)
{
	struct buf *super_buf;
	struct minix_v1_superblock *super_v1;

	super_buf = get_block(dev, MINIX_SUPER_ZONE);
	if (!super_buf)
		return -1;
	super_v1 = (struct minix_v1_superblock *) super_buf->block;

	// TODO we assume this is not a disk yet, so manually initialize it
	memset_s(super_buf->block, 0x00, MINIX_V1_ZONE_SIZE);
	super_v1->num_inodes = 0x40;
	super_v1->num_zones = 0x64;
	super_v1->imap_blocks = 1;
	super_v1->zmap_blocks = 1;
	super_v1->first_zone = 6;
	super_v1->log_zone_size = MINIX_V1_LOG_ZONE_SIZE;
	super_v1->max_file_size;	//?????
	super_v1->magic = 0x137F;
	super_v1->state = 0x0001;

	release_block(super_buf, BCF_DIRTY);

	// Initialize bitmap zones
	bitmap_init(dev, MINIX_V1_INODE_BITMAP_START(super_v1), super_v1->imap_blocks, super_v1->num_inodes, 1);
	bitmap_init(dev, MINIX_V1_ZONE_BITMAP_START(super_v1), super_v1->zmap_blocks, super_v1->num_zones, super_v1->first_zone);

	// Zero the inode table
	for (int i = 0; i < (super_v1->num_inodes >> MINIX_V1_LOG_INODES_PER_ZONE); i++) {
		struct buf *inode_buf = get_block(dev, MINIX_V1_INODE_TABLE_START(super_v1) + i);
		if (!inode_buf)
			return ENOMEM;
		memset_s(inode_buf->block, 0x00, MINIX_V1_ZONE_SIZE);
		release_block(inode_buf, BCF_DIRTY);
	}


	// Initialize root inode
	bitnum_t root_ino = alloc_inode(super_v1, dev, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO, SU_UID, 0);
	if (root_ino != 1) {
		printk("Failed to init\n");
		return -100;
	}

	zone_t zone = dir_create_data(super_v1, dev, root_ino, root_ino);
	if (!zone)
		return ENOMEM;

	zone_t offset = root_ino >> MINIX_V1_LOG_INODES_PER_ZONE;
	struct buf *inode_buf = get_block(dev, MINIX_V1_INODE_TABLE_START(super_v1) + offset);
	if (!inode_buf)
		return ENOMEM;

	struct minix_v1_inode *inode_table = inode_buf->block;
	inode_table[root_ino].zones[0] = zone;
	release_block(inode_buf, BCF_DIRTY);

	//sync_bufcache();

	return 0;
}

#endif
