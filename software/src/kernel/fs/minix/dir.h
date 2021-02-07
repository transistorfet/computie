
#ifndef _SRC_KERNEL_FS_MINIX_DIR_H
#define _SRC_KERNEL_FS_MINIX_DIR_H

#include <asm/macros.h>

#include "minix.h"
#include "inodes.h"
#include "zones.h"


static struct vnode *dir_setup(struct vnode *vnode, struct vnode *parent)
{
	zone_t zone;
	struct buf *buf;
	struct minix_v1_dirent *entries;

	zone = minix_alloc_zone(vnode->mp->super);
	buf = get_block(vnode->mp->dev, zone);
	if (!buf)
		return 0;

	entries = (struct minix_v1_dirent *) buf->block;

	entries[0].inode = to_le16((minix_v1_inode_t) vnode->ino);
	strcpy(entries[0].filename, ".");

	entries[1].inode = to_le16(parent ? (minix_v1_inode_t) vnode->ino : 1);
	strcpy(entries[1].filename, "..");

	release_block(buf, BCF_DIRTY);

	MINIX_DATA(vnode).zones[0] = to_le16(zone);
	// Set the initial directory size to include the two default entries
	vnode->size = sizeof(struct minix_v1_dirent) << 1;
	mark_vnode_dirty(vnode);

	return vnode;
}

static short dir_is_empty(struct vnode *vnode)
{
	zone_t zone;
	struct buf *buf;
	struct minix_v1_dirent *entries;

	for (zone_t znum = 0; zone = zone_lookup(vnode, znum, MFS_LOOKUP_ZONE); znum++) {
		buf = get_block(vnode->mp->dev, zone);
		if (!buf)
			return EIO;
		entries = (struct minix_v1_dirent *) buf->block;
		for (short i = 0; i < MINIX_V1_DIRENTS_PER_ZONE; i++) {
			if (entries[i].inode && strcmp(entries[i].filename, ".") && strcmp(entries[i].filename, "..")) {
				release_block(buf, 0);
				return 0;
			}
		}
		release_block(buf, 0);
	}
	return 1;
}

static struct minix_v1_dirent *dir_find_entry_by_inode(struct vnode *dir, inode_t ino, char create, struct buf **result)
{
	zone_t zone;
	struct buf *buf;
	struct minix_v1_dirent *entries;

	for (zone_t znum = 0; zone = zone_lookup(dir, znum, create); znum++) {
		buf = get_block(dir->mp->dev, zone);
		if (!buf)
			return NULL;
		entries = (struct minix_v1_dirent *) buf->block;
		for (short i = 0; i < MINIX_V1_DIRENTS_PER_ZONE; i++) {
			if (from_le16(entries[i].inode) == ino) {
				offset_t position = ((znum << MINIX_V1_LOG_DIRENTS_PER_ZONE) + i + 1) << __builtin_ctz(sizeof(struct minix_v1_dirent));
				if (dir->size < position) {
					// TODO this is sort of a hack, isn't it?  Also what about shrinking in size
					dir->size = position;
					mark_vnode_dirty(dir);
				}
				*result = buf;
				return &entries[i];
			}
		}
		release_block(buf, 0);
	}
	return NULL;
}

static struct minix_v1_dirent *dir_find_entry_by_name(struct vnode *dir, const char *filename, char create, struct buf **result)
{
	zone_t zone;
	struct buf *buf;
	struct minix_v1_dirent *entries;

	for (zone_t znum = 0; zone = zone_lookup(dir, znum, create); znum++) {
		buf = get_block(dir->mp->dev, zone);
		if (!buf)
			return NULL;
		entries = (struct minix_v1_dirent *) buf->block;
		for (short i = 0; i < MINIX_V1_DIRENTS_PER_ZONE; i++) {
			if (entries[i].inode && !strcmp(filename, entries[i].filename)) {
				*result = buf;
				return &entries[i];
			}
		}
		release_block(buf, 0);
	}
	return NULL;
}


static struct minix_v1_dirent *dir_alloc_entry(struct vnode *vnode, const char *filename, struct buf **result)
{
	struct minix_v1_dirent *dir;

	dir = dir_find_entry_by_inode(vnode, 0, MFS_CREATE_ZONE, result);
	if (!dir)
		return NULL;

	strncpy(dir->filename, filename, MINIX_V1_MAX_FILENAME);
	dir->filename[MINIX_V1_MAX_FILENAME - 1] = '\0';

	return dir;
}

#endif
