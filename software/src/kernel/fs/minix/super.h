
#ifndef _SRC_KERNEL_FS_MINIX_SUPER_H
#define _SRC_KERNEL_FS_MINIX_SUPER_H

#include "../bufcache.h"

#include "minix.h"
#include "inodes.h"
#include "mkfs.h"

static struct minix_super *load_superblock(device_t dev)
{
	struct buf *super_buf;
	struct minix_super *super;
	struct minix_v1_superblock *super_v1;

	super_buf = get_block(dev, MINIX_SUPER_ZONE);
	if (!super_buf)
		return NULL;
	super_v1 = (struct minix_v1_superblock *) super_buf->block;


	// TODO this is a temporary hack for cold starting a ram disk
	if (super_v1->magic != 0x137F) {
		printk("Initializing disk\n");
		if (minix_mkfs(dev))
			return NULL;
	}


	super = kmalloc(sizeof(struct minix_super));
	memcpy_s(&super->super_v1, super_v1, sizeof(struct minix_v1_superblock));
	super->dev = dev;
	super->max_filename = MINIX_V1_MAX_FILENAME;

	return super;
}

static void free_superblock(struct minix_super *super)
{
	kmfree(super);
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

