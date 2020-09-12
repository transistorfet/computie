
#ifndef _SRC_KERNEL_FS_MINIX_MINIX_V1_H
#define _SRC_KERNEL_FS_MINIX_MINIX_V1_H

#include <stdint.h>

#define MINIX_V1_ZONE_SIZE		1024

#define	MINIX_V1_BITS_PER_ZONE		(MINIX_V1_ZONE_SIZE * 8)

#define MINIX_V1_INODE_ZONES		9
#define MINIX_V1_MAX_FILENAME		14

// TODO this is temporary until I have a proper way of supporting different versions
typedef uint16_t minix_zone_t;
//typedef uint16_t minix_v1_zone_t;

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
	uint16_t zones[MINIX_V1_INODE_ZONES];
};

struct minix_v1_dirent {
	uint16_t inode;
	char filename[MINIX_V1_MAX_FILENAME];
};

#endif

