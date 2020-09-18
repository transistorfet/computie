
#ifndef _SRC_KERNEL_FS_MALLOCFS_ZONES_H
#define _SRC_KERNEL_FS_MALLOCFS_ZONES_H

#include "mallocfs.h"
#include "alloc.h"

#define MFS_LOOKUP_ZONE		0
#define MFS_CREATE_ZONE		1

static struct mallocfs_block *zone_lookup(struct vnode *vnode, zone_t znum, char create)
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
	struct mallocfs_block *zone;

	// TODO this can only be used with files or empty directories

	for (zone_t znum = 0; zone = zone_lookup(vnode, znum, MFS_LOOKUP_ZONE); znum++)
		MFS_FREE_BLOCK(zone);

	for (char i = MALLOCFS_TIER1_ZONES; i < MALLOCFS_TOTAL_ZONES; i++) {
		if (MALLOCFS_DATA(vnode).zones[i])
			MFS_FREE_BLOCK(MALLOCFS_DATA(vnode).zones[i]);
	}

	for (char i = 0; i < MALLOCFS_TIER1_ZONES; i++)
		MALLOCFS_DATA(vnode).zones[i] = NULL;
}

#endif
