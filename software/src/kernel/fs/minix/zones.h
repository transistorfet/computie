
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
	buf = get_block(super->dev, bit);
	if (!buf)
		return NULL;

	memset_s(buf->block, 0, MINIX_V1_ZONE_SIZE);
	release_block(buf, BCF_DIRTY);

	return bit;
}

static void minix_free_zone(struct minix_super *super, zone_t zonenum)
{
	bit_free(super->dev, MINIX_V1_ZONE_BITMAP_START(&super->super_v1), zonenum);
}

static zone_t zone_lookup(struct vnode *vnode, zone_t znum, char create)
{
	zone_t ret;
	minix_v1_zone_t *zone;
	struct buf *buf = NULL;
	minix_v1_zone_t *zones = MINIX_DATA(vnode).zones;
	struct minix_super *super = MINIX_SUPER(vnode->mp->super);

	if (znum < MINIX_V1_TIER1_ZONENUMS)
		zone = &zones[znum];
	else {
		znum -= MINIX_V1_TIER1_ZONENUMS;
		char t1_zone = MINIX_V1_TIER1_ZONENUMS + (znum >> MINIX_V1_LOG_ZONENUMS_PER_ZONE);
		short t2_zone = znum & (MINIX_V1_ZONENUMS_PER_ZONE - 1);

		// Ran out of tier 1 zones (file size limit)
		if (t1_zone >= MINIX_V1_TOTAL_ZONENUMS)
			return 0;

		// Either quit or create a new empty tier2 zone table
		if (!zones[t1_zone]) {
			if (create)
				zones[t1_zone] = minix_alloc_zone(super);
			else
				return 0;
		}

		buf = get_block(super->dev, zones[t1_zone]);
		if (!buf)
			return 0;
		zone = &((minix_v1_zone_t *) &buf->block)[t2_zone];
	}

	if (create && !*zone) {
		*zone = minix_alloc_zone(super);
		if (buf)
			mark_block_dirty(buf);
		mark_vnode_dirty(vnode);
	}
	ret = (zone_t) *zone;
	if (buf)
		release_block(buf, 0);
	return ret;
}

static void zone_free_all(struct vnode *vnode)
{
	zone_t zone;

	// NOTE this can only be used with files or empty directories

	for (zone_t znum = 0; zone = zone_lookup(vnode, znum, MFS_LOOKUP_ZONE); znum++)
		minix_free_zone(MINIX_SUPER(vnode->mp->super), zone);

	for (char i = MINIX_V1_TIER1_ZONENUMS; i < MINIX_V1_TOTAL_ZONENUMS; i++) {
		if (MINIX_DATA(vnode).zones[i])
			minix_free_zone(MINIX_SUPER(vnode->mp->super), MINIX_DATA(vnode).zones[i]);
	}

	for (char i = 0; i < MINIX_V1_TOTAL_ZONENUMS; i++)
		MINIX_DATA(vnode).zones[i] = 0;

	mark_vnode_dirty(vnode);
}

#endif
