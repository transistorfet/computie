
#ifndef _SRC_KERNEL_FS_MINIX_BITMAPS_H
#define _SRC_KERNEL_FS_MINIX_BITMAPS_H

#include "minix.h"
#include "../bufcache.h"

/*
struct bitmap {
	device_t dev;
	minix_zone_t start;
	minix_zone_t end;
	short words_per_zone;
};
*/

typedef int bitnum_t;

static inline char bit_mask(char bits)
{
	char byte = 0x00;
	for (short j = 0; j < bits; j++)
		byte = (byte << 1) | 0x01;
	return byte;
}

int bitmap_init(device_t dev, zone_t bitmap_start, int bitmap_size, int num_entries, short reserve);
bitnum_t bit_alloc(device_t dev, zone_t bitmap_start, int bitmap_size, zone_t near);
int bit_free(device_t dev, zone_t bitmap_start, bitnum_t bitnum);

#endif
