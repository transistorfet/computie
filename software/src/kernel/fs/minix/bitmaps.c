
#include <string.h>

#include "minix.h"
#include "bitmaps.h"


int bitmap_init(device_t dev, minix_zone_t bitmap_start, int bitmap_size, int num_entries, short reserve)
{
	char *block;
	struct buf *buf;

	// Set only num_entries bits as free, and the rest of the block as reserved (unallocatable)
	for (short i = bitmap_start; i < bitmap_start + bitmap_size; i++) {
		buf = get_block(dev, i);
		if (!buf)
			return -1;
		block = buf->block;

		if (num_entries < MINIX_V1_BITS_PER_ZONE) {
			int bytes = (num_entries >> 3);
			char bits = (num_entries & 0x07);

			if (bits)
				bytes += 1;
			memset_s(block, 0x00, bytes);
			if (bits)
				block[bytes - 1] = ~bit_mask(bits);
			memset_s(&block[bytes], 0xFF, MINIX_V1_ZONE_SIZE - bytes);
			break;
		}
		else
			memset_s(block, 0x00, MINIX_V1_ZONE_SIZE);
		num_entries -= MINIX_V1_BITS_PER_ZONE;

		release_block(buf, BCF_DIRTY);
	}

	buf = get_block(dev, bitmap_start);
	if (!buf)
		return -1;
	block = buf->block;

	// Reserve entries at the beginning of table
	short i = 0;
	for (; i < (reserve >> 3); i++)
		block[i] = 0xFF;
	block[i] = bit_mask(reserve & 0x7);

	release_block(buf, BCF_DIRTY);
	return 0;
}

minix_zone_t bit_alloc(device_t dev, minix_zone_t bitmap_start, int bitmap_size, minix_zone_t near)
{
	char bit;
	char *block;
	int zone = 0;
	struct buf *buf;

	for (; zone < bitmap_size; zone++) {
		buf = get_block(dev, bitmap_start + zone);
		if (!buf)
			return 0;
		block = buf->block;

		for (int i = 0; i < MINIX_V1_ZONE_SIZE; i++) {
			//printk("Bitsearch %d: %x\n", i, block[i]);
			if ((char) ~block[i]) {
				for (bit = 0; bit < 8 && ((0x01 << bit) & block[i]); bit++) { }
				block[i] |= (0x01 << bit);
				release_block(buf, BCF_DIRTY);
				return bit + (i * 8) + (zone * MINIX_V1_ZONE_SIZE * 8);
			}
		}

		release_block(buf, 0);
	}

	return 0;
}

int bit_free(device_t dev, minix_zone_t bitmap_start, minix_zone_t zonenum)
{
	minix_zone_t zone = (zonenum >> 3) / MINIX_V1_ZONE_SIZE;
	int i = (zonenum >> 3);
	char bit = (zonenum & 0x7);
	struct buf *buf = get_block(dev, bitmap_start + zone);
	if (!buf)
		return -1;
	char *block = buf->block;

	block[i] &= ~(0x01 << bit);
	release_block(buf, BCF_DIRTY);
	return 0;
}

