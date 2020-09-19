
#ifndef _SRC_KERNEL_FS_MINIX_SUPER_H
#define _SRC_KERNEL_FS_MINIX_SUPER_H

#include "../bufcache.h"

#include "minix.h"
#include "inodes.h"





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
	// TODO you need to calculate the number of zones the inode table covers in order to properly zero this
	//for (int i = 0; i < **INODE_ZONES**; i++) {
	struct buf *inode_buf = get_block(dev, MINIX_V1_INODE_TABLE_START(super_v1));
	if (!inode_buf)
		return ENOMEM;
	memset_s(inode_buf->block, 0x00, MINIX_V1_ZONE_SIZE);
	release_block(inode_buf, BCF_DIRTY);

	// Initialize root inode
	bitnum_t inode_num = alloc_inode(super_v1, dev, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO, SU_UID, 0);
	if (inode_num != 1)
		return -100;

	//sync_bufcache();

	return 0;
}

static struct minix_super *load_superblock(device_t dev)
{
	struct buf *super_buf;
	struct minix_super *super;
	struct minix_v1_superblock *super_v1;

	// TODO we assume this is not a disk yet, so manually initialize it
	if (minix_mkfs(dev))
		return NULL;

	super_buf = get_block(dev, MINIX_SUPER_ZONE);
	if (!super_buf)
		return NULL;
	super_v1 = (struct minix_v1_superblock *) super_buf->block;

	super = kmalloc(sizeof(struct minix_super));
	memcpy_s(&super->super_v1, super_v1, sizeof(struct minix_v1_superblock));
	super->dev = dev;
	super->max_filename = MINIX_V1_MAX_FILENAME;


	/*
	int zone;
	for (int i = 0; i < 102; i++) {
		zone = bit_alloc(dev, MINIX_BITMAP_ZONES + super->imap_blocks, super->zmap_blocks, 0);
		printk("Alloced %d\n", zone);
	}

	bit_free(dev, MINIX_BITMAP_ZONES, 19);
	bit_free(dev, MINIX_BITMAP_ZONES, 52);
	*/

	return super;
}

static zone_t minix_alloc_zone(struct minix_super *super)
{
	bitnum_t bit;
	struct buf *buf;
	struct minix_block *block;

	bit = bit_alloc(super->dev, MINIX_V1_ZONE_BITMAP_START(&super->super_v1), super->super_v1.zmap_blocks, 0);
	if (!bit)
		return NULL;
	buf = get_block(super->dev, bit);
	if (!buf)
		return NULL;

	memset_s(buf->block, 0, MINIX_V1_ZONE_SIZE);
	release_block(buf, BCF_DIRTY);

	return bit;
}

static void minix_free_zone(struct minix_super *super, zone_t zonenum)
{
	bit_free(super->dev, MINIX_V1_ZONE_BITMAP_START(&super->super_v1), zonenum);
}



/*
#include "../../byteorder.h"

static inline void _adjust_superblock(struct minix_superblock *super);
static inline void _adjust_inode(struct minix_inode *inode);

static inline void _adjust_superblock(struct minix_superblock *super)
{
	for (char i = 0; i < (sizeof(struct minix_superblock) >> 1); i++)
		BYTEORDER_SWAP_SHORT(((uint16_t *) &super)[i]);
	BYTEORDER_SWAP_INSTR(super->max_file_size);
}

static inline void _adjust_inode(struct minix_inode *inode)
{
	BYTEORDER_SWAP_SHORT(inode->mode);
	BYTEORDER_SWAP_SHORT(inode->uid);
	BYTEORDER_SWAP_LONG(inode->size);
	BYTEORDER_SWAP_LONG(inode->mtime);
	for (char i = 0; i < MINIX_INODE_ZONES; i++)
		BYTEORDER_SWAP_SHORT(inode->zones[i]);
}

*/


#endif

