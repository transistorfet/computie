
#ifndef _SRC_KERNEL_FS_MINIX_MINIX_V1_H
#define _SRC_KERNEL_FS_MINIX_MINIX_V1_H

#include <stdint.h>

#define MINIX_V1_ZONE_SIZE		1024
#define MINIX_V1_LOG_ZONE_SIZE		__builtin_ctz(MINIX_V1_ZONE_SIZE)
#define MINIX_V1_ZONENUMS_PER_ZONE	(MINIX_V1_ZONE_SIZE / sizeof(minix_v1_zone_t))
#define MINIX_V1_LOG_ZONENUMS_PER_ZONE	__builtin_ctz(MINIX_V1_ZONENUMS_PER_ZONE)
#define MINIX_V1_DIRENTS_PER_ZONE	(MINIX_V1_ZONE_SIZE / sizeof(struct minix_v1_dirent))
#define MINIX_V1_LOG_DIRENTS_PER_ZONE	__builtin_ctz(MINIX_V1_DIRENTS_PER_ZONE)
#define MINIX_V1_INODES_PER_ZONE	(MINIX_V1_ZONE_SIZE / sizeof(struct minix_v1_inode))
#define MINIX_V1_LOG_INODES_PER_ZONE	__builtin_ctz(MINIX_V1_INODES_PER_ZONE)

#define	MINIX_V1_BITS_PER_ZONE		(MINIX_V1_ZONE_SIZE * 8)

#define MINIX_V1_MAX_FILENAME		14

#define MINIX_V1_INODE_ZONENUMS		9
#define MINIX_V1_TIER1_ZONENUMS		7
#define MINIX_V1_INDIRECT_TIERS		2
#define MINIX_V1_TOTAL_ZONENUMS		MINIX_V1_TIER1_ZONENUMS + MINIX_V1_INDIRECT_TIERS

#define MINIX_V1_INODE_BITMAP_START(super_v1)	(MINIX_BITMAP_ZONES)
#define MINIX_V1_ZONE_BITMAP_START(super_v1)	(MINIX_BITMAP_ZONES + (super_v1)->imap_blocks)
#define MINIX_V1_INODE_TABLE_START(super_v1)	(MINIX_BITMAP_ZONES + (super_v1)->imap_blocks + (super_v1)->zmap_blocks)



typedef uint16_t minix_v1_zone_t;
typedef uint16_t minix_v1_inode_t;


struct minix_v1_superblock {
	uint16_t num_inodes;
	uint16_t num_zones;
	uint16_t imap_blocks;
	uint16_t zmap_blocks;
	uint16_t first_zone;
	uint16_t log_zone_size;
	uint32_t max_file_size;
	uint16_t magic;
	uint16_t state;
};

struct minix_v1_inode {
	uint16_t mode;
	uint16_t uid;
	uint32_t size;
	uint32_t mtime;
	uint8_t gid;
	uint8_t nlinks;
	minix_v1_zone_t zones[MINIX_V1_INODE_ZONENUMS];
};

struct minix_v1_dirent {
	minix_v1_inode_t inode;
	char filename[MINIX_V1_MAX_FILENAME];
};

#endif

