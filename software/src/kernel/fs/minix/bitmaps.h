
#ifndef _SRC_KERNEL_FS_MINIX_BITMAPS_H
#define _SRC_KERNEL_FS_MINIX_BITMAPS_H

#include "../bufcache.h"

/*
struct bitmap {
	device_t dev;
	minix_zone_t start;
	minix_zone_t end;
	short words_per_zone;
};
*/



minix_zone_t bit_alloc(device_t dev, minix_zone_t bitmap_start, int bitmap_size, minix_zone_t near)
{
	char bit;
	int zone = 0;
	uint16_t *block;
	struct buf *buf;

	for (; zone < bitmap_size; zone++) {
		buf = get_block(dev, bitmap_start + zone);
		block = (uint16_t *) buf->block;

		for (int i = 0; i < (MINIX_ZONE_SIZE / 2); i++) {
			//printk("Bitsearch %d: %x\n", i, block[i]);
			if (~block[i] & 0xFFFF) {
				for (bit = 0; bit < 16 && ((0x01 << bit) & block[i]); bit++) { }
				block[i] |= (0x01 << bit);
				mark_block_dirty(buf);
				return bit + (i * 16) + (zone * MINIX_ZONE_SIZE * 8);
			}
		}
	}

	return 0;
}

void bit_free(device_t dev, minix_zone_t bitmap_start, minix_zone_t zonenum)
{
	minix_zone_t zone = (zonenum >> 3) / MINIX_ZONE_SIZE;
	int i = (zonenum >> 4);
	char bit = (zonenum & 0xf);
	struct buf *buf = get_block(dev, bitmap_start + zone);
	uint16_t *block = (uint16_t *) buf->block;

	block[i] &= ~(0x01 << bit);
	mark_block_dirty(buf);
}

#endif
