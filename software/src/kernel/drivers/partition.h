
#ifndef _SRC_KERNEL_DRIVERS_PARTITION_H
#define _SRC_KERNEL_DRIVERS_PARTITION_H

#include <stdint.h>

#define PARTITION_MAX       4

typedef unsigned long sector_t;

struct partition {
	sector_t base;
	sector_t size;
	uint8_t	fstype;
	uint8_t flags;
};

int read_partition_table(char *buffer, struct partition *devices);

#endif
