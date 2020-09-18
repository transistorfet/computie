
#ifndef _SRC_KERNEL_FS_MALLOCFS_DIR_H
#define _SRC_KERNEL_FS_MALLOCFS_DIR_H

#include "mallocfs.h"
#include "alloc.h"

static struct vnode *dir_setup(struct vnode *vnode, struct vnode *parent)
{
	struct mallocfs_block *block;

	block = new_mallocfs_block(MFS_ZERO);
	if (!block) {
		vfs_release_vnode(vnode);
		return NULL;
	}
	MALLOCFS_DATA(vnode).zones[0] = block;

	block->entries[0].vnode = vnode;
	strcpy(block->entries[0].name, ".");

	// Set .. to point to the parent, unless it is NULL, in which case this is the root node and will point to itself
	block->entries[1].vnode = parent ? parent : vnode;
	strcpy(block->entries[1].name, "..");

	return vnode;
}

static short dir_is_empty(struct vnode *vnode)
{
	struct mallocfs_block *zone;

	for (zone_t znum = 0; zone = zone_lookup(vnode, znum, MFS_LOOKUP_ZONE); znum++) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode && strcmp(zone->entries[i].name, ".") && strcmp(zone->entries[i].name, ".."))
				return 0;
		}
	}
	return 1;
}

static struct mallocfs_dirent *dir_find_entry_by_vnode(struct vnode *dir, struct vnode *file, char create)
{
	struct mallocfs_block *zone;

	for (zone_t znum = 0; zone = zone_lookup(dir, znum, create); znum++) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode == file)
				return &zone->entries[i];
		}
	}
	return NULL;
}

static struct mallocfs_dirent *dir_find_entry_by_name(struct vnode *dir, const char *filename, char create)
{
	struct mallocfs_block *zone;

	for (zone_t znum = 0; zone = zone_lookup(dir, znum, create); znum++) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode && !strcmp(filename, zone->entries[i].name))
				return &zone->entries[i];
		}
	}
	return NULL;
}


static struct mallocfs_dirent *dir_alloc_entry(struct vnode *vnode, const char *filename)
{
	struct mallocfs_dirent *dir;

	dir = dir_find_entry_by_vnode(vnode, NULL, MFS_CREATE_ZONE);
	if (!dir)
		return NULL;

	strncpy(dir->name, filename, MALLOCFS_MAX_FILENAME);
	dir->name[MALLOCFS_MAX_FILENAME - 1] = '\0';

	return dir;
}

#endif
