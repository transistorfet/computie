
#ifndef _SRC_KERNEL_FS_MALLOCFS_ZONES_H
#define _SRC_KERNEL_FS_MALLOCFS_ZONES_H

#include "mallocfs.h"
#include "alloc.h"

#define MFS_CREATE_ZONE		1

struct zone_iter {
	char t1_zone;
	short t2_zone;
};

static inline void zone_iter_start(struct zone_iter *iter)
{
	iter->t1_zone = 0;
	iter->t2_zone = 0;
}

static struct mallocfs_block *zone_iter_next(struct zone_iter *iter, struct mallocfs_block **zones)
{
	struct mallocfs_block *zone; 

	if (iter->t1_zone < MALLOCFS_TIER1_ZONES)
		zone = zones[iter->t1_zone];
	else if (iter->t1_zone < MALLOCFS_TOTAL_ZONES && zones[iter->t1_zone])
		zone = zones[iter->t1_zone]->zones[iter->t2_zone];
	else
		zone = NULL;

	if (!zone)
		return NULL;

	if (iter->t1_zone < MALLOCFS_TIER1_ZONES)
		iter->t1_zone++;
	else if (iter->t2_zone < MALLOCFS_BLOCK_ZONES - 1)
		iter->t2_zone++;
	else if (iter->t1_zone < MALLOCFS_TOTAL_ZONES) {
		iter->t1_zone++;
		iter->t2_zone = 0;
	}

	return zone;
}

static struct mallocfs_block *zone_lookup(struct vnode *vnode, mallocfs_zone_t znum, char create)
{
	struct mallocfs_block **zone;
	struct mallocfs_block **zones = MALLOCFS_DATA(vnode).zones;

	if (znum < MALLOCFS_TIER1_ZONES)
		zone = &zones[znum];
	else {
		znum -= MALLOCFS_TIER1_ZONES;
		char t1_zone = MALLOCFS_TIER1_ZONES + (znum >> MALLOCFS_LOG_BLOCK_ZONES);
		short t2_zone = znum & (MALLOCFS_BLOCK_ZONES - 1);

		// Ran out of tier 1 zones (file size limit)
		if (t1_zone >= MALLOCFS_TOTAL_ZONES)
			return NULL;

		// Either quit or create a new empty tier2 zone table
		if (!zones[t1_zone]) {
			if (create)
				zones[t1_zone] = new_mallocfs_block(MFS_ZERO);
			else
				return NULL;
		}

		zone = &zones[t1_zone]->zones[t2_zone];
	}

	if (create && !*zone)
		*zone = new_mallocfs_block(MFS_ZERO);
	return *zone;
}

static struct mallocfs_block *zone_free_all(struct vnode *vnode)
{
	struct zone_iter iter;
	struct mallocfs_block *zone;

	zone_iter_start(&iter);
	while ((zone = zone_iter_next(&iter, MALLOCFS_DATA(vnode).zones))) {
		if (zone)
			MFS_FREE_BLOCK(zone);
	}

	for (char i = MALLOCFS_TIER1_ZONES; i < MALLOCFS_TOTAL_ZONES; i++) {
		if (MALLOCFS_DATA(vnode).zones[i])
			MFS_FREE_BLOCK(MALLOCFS_DATA(vnode).zones[i]);
	}
}

#endif
