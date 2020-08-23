
#include <kernel/vfs.h>
#include <kernel/printk.h>
 
#include "minix.h"
#include "bitmaps.h"

#include "../bufcache.h"


struct minix_superblock *super;


int init_minix()
{
	device_t dev = 1;
	struct buf *super_buf;

	super_buf = get_block(dev, MINIX_SUPER_ZONE);
	if (!super_buf)
		return -1;
	super = (struct minix_superblock *) super_buf->block;

	// TODO we assume this is not a disk yet, so manually initialize it
	super->num_inodes = 40;
	super->num_zones = 64;
	super->imap_blocks = 1;
	super->zmap_blocks = 1;
	super->first_zone = 6;
	super->log_zone_size;	//?????
	super->max_file_size;	//?????
	super->magic = 0x137F;
	super->state = 0x0001;

	mark_block_dirty(super_buf);


	// Zero the bitmap zones to start
	for (short i = MINIX_BITMAP_ZONES; i < MINIX_BITMAP_ZONES + super->imap_blocks + super->zmap_blocks; i++) {
		struct buf *buf = get_block(dev, i);
		char *block = buf->block;
		memset_s(block, '\0', MINIX_ZONE_SIZE);
		mark_block_dirty(buf);
	}


	int zone;
	for (int i = 0; i < 20; i++)
		zone = bit_alloc(dev, MINIX_BITMAP_ZONES, super->imap_blocks, 0);

	bit_free(dev, MINIX_BITMAP_ZONES, 19);
	bit_free(dev, MINIX_BITMAP_ZONES, 52);

	sync_bufcache();

	return 0;
}
























/*
static void _minix_read_superblock(struct mount *mount)
{
	struct minix_superblock *super;

	// TODO load super
	super = get_block(mount->root->device, 1);

	//_adjust_superblock(super);

}
*/


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
