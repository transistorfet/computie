
#ifndef _SRC_KERNEL_FS_MINIX_ZONES_H
#define _SRC_KERNEL_FS_MINIX_ZONES_H

#include "minix.h"

#define MFS_LOOKUP_ZONE		0
#define MFS_CREATE_ZONE		1


static zone_t minix_alloc_zone(struct minix_super *super)
{
	bitnum_t bit;
	struct buf *buf;
	struct minix_block *block;

	bit = bit_alloc(super->dev, MINIX_V1_ZONE_BITMAP_START(&super->super_v1), super->super_v1.zmap_blocks, 0);
	if (!bit)
		return NULL;
	bit += super->super_v1.first_zone - 1;
	buf = get_block(super->dev, bit);
	if (!buf)
		return NULL;

	memset_s(buf->block, 0, MINIX_V1_ZONE_SIZE);
	release_block(buf, BCF_DIRTY);

	return bit;
}

static void minix_free_zone(struct minix_super *super, zone_t zonenum)
{
	bit_free(super->dev, MINIX_V1_ZONE_BITMAP_START(&super->super_v1), zonenum - super->super_v1.first_zone + 1);
}


static inline char zone_calculate_tier(zone_t *tiers, zone_t znum)
{
	char tier = 0;

	if (znum < MINIX_V1_TIER1_ZONENUMS) {
		tiers[0] = znum;
		return 1;
	}

	znum -= MINIX_V1_TIER1_ZONENUMS;
	tiers[0] = MINIX_V1_TIER1_ZONENUMS;
	if (znum < MINIX_V1_ZONENUMS_PER_ZONE) {
		tiers[1] = znum;
		return 2;
	}

	znum -= MINIX_V1_ZONENUMS_PER_ZONE;
	tiers[0]++;
	if (znum < MINIX_V1_ZONENUMS_PER_ZONE * MINIX_V1_ZONENUMS_PER_ZONE) {
		tiers[1] = (znum >> MINIX_V1_LOG_ZONENUMS_PER_ZONE);
		tiers[2] = znum & (MINIX_V1_ZONENUMS_PER_ZONE - 1);
		return 3;
	}

	// TODO a 4th tier?
	return -1;
}

static zone_t zone_lookup(struct vnode *vnode, zone_t znum, char create)
{
	zone_t ret;
	char ntiers;
	minix_v1_zone_t *zone;
	zone_t tiers[MINIX_V1_TIERS];
	struct buf *buf = NULL;
	struct minix_super *super = MINIX_SUPER(vnode->mp->super);

	ntiers = zone_calculate_tier(tiers, znum);
	if (ntiers < 0)
		return 0;

	for (char tier = 0; tier < ntiers; tier++) {
		if (tier == 0)
			zone = &MINIX_DATA(vnode).zones[tiers[0]];
		else {
			if (buf)
				release_block(buf, 0);
			buf = get_block(super->dev, from_le16(*zone));
			if (!buf)
				return 0;

			zone = &MINIX_V1_ZONENUM_TABLE(buf->block)[tiers[tier]];
		}

		if (!*zone) {
			if (create) {
				*zone = to_le16(minix_alloc_zone(super));
				if (buf)
					mark_block_dirty(buf);
			}
			else {
				if (buf)
					release_block(buf, 0);
				return 0;
			}
		}
	}

	ret = (zone_t) from_le16(*zone);
	if (buf)
		release_block(buf, 0);
	return ret;
}

static void zone_free_all(struct vnode *vnode)
{
	zone_t zone;

	// NOTE this can only be used with files or empty directories

	// If this is a device file, then the zone is the device number, so just return
	if (vnode->mode & S_IFCHR)
		return;

	// Traverse all zones and free each
	for (zone_t znum = 0; zone = zone_lookup(vnode, znum, MFS_LOOKUP_ZONE); znum++)
		minix_free_zone(MINIX_SUPER(vnode->mp->super), zone);

	// Go through the tier 2 zonenum tables and free each
	if (MINIX_DATA(vnode).zones[8]) {
		struct buf *buf = get_block(MINIX_SUPER(vnode->mp->super)->dev, from_le16(MINIX_DATA(vnode).zones[8]));
		if (buf) {
			minix_v1_zone_t *entries = buf->block;
			for (zone_t i = 0; i < MINIX_V1_ZONENUMS_PER_ZONE; i++) {
				if (entries[i])
					minix_free_zone(MINIX_SUPER(vnode->mp->super), from_le16(entries[i]));
			}
			release_block(buf, 0);
		}
	}

	// Go through the tier 1 zonenum tables and free each
	for (char i = MINIX_V1_TIER1_ZONENUMS; i < MINIX_V1_TOTAL_ZONENUMS; i++) {
		if (MINIX_DATA(vnode).zones[i])
			minix_free_zone(MINIX_SUPER(vnode->mp->super), from_le16(MINIX_DATA(vnode).zones[i]));
	}

	// Go through the tier 0 zonenum tables (the inode zones) and free each
	for (char i = 0; i < MINIX_V1_TOTAL_ZONENUMS; i++)
		MINIX_DATA(vnode).zones[i] = 0;

	mark_vnode_dirty(vnode);
}

#endif
