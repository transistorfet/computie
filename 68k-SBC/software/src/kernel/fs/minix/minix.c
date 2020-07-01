 
#include "minix.h"
#include "../../byteorder.h"

static inline void _adjust_superblock(struct minix_superblock *super);
static inline void _adjust_inode(struct minix_inode *inode);



void minix_read_superblock()
{
	struct minix_superblock super;

	// TODO load super


	_adjust_superblock(&super);

}


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
		BYTEORDER_SWAP_LONG(inode->zones[i]);
}
