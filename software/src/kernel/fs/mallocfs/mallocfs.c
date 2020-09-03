
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/driver.h>
 
#include "mallocfs.h"
#include "alloc.h"
#include "zones.h"



struct vfile_ops mallocfs_vfile_ops = {
	mallocfs_open,
	mallocfs_close,
	mallocfs_read,
	mallocfs_write,
	mallocfs_ioctl,
	mallocfs_seek,
	mallocfs_readdir,
};

struct vnode_ops mallocfs_vnode_ops = {
	&mallocfs_vfile_ops,
	mallocfs_create,
	mallocfs_mknod,
	mallocfs_lookup,
	mallocfs_unlink,
	mallocfs_truncate,
	mallocfs_release,
};


// TODO this will be stored in the mount
struct vnode *mallocfs_root;

static struct mallocfs_dirent *_alloc_dirent(struct vnode *vnode, const char *filename);
static struct mallocfs_dirent *_find_empty_dirent(struct vnode *dir);
static struct mallocfs_dirent *_find_dirent(struct vnode *dir, struct vnode *file);
static short _is_empty_dirent(struct vnode *vnode);

int init_mallocfs()
{
	init_mallocfs_alloc();

	mallocfs_root = new_mallocfs_vnode_with_block(S_IFDIR | 0755, NULL);
	// Set the parent entry (..) to be the root vnode
	// TODO This wouldn't be the case for a mounted filesystem
	MALLOCFS_DATA(mallocfs_root).zones[0]->entries[1].vnode = mallocfs_root;
}

int mallocfs_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result)
{
	struct vnode *newnode;
	struct mallocfs_dirent *dir;
	struct mallocfs_block *block;

	if (!(vnode->mode & S_IFDIR))
		return ENOTDIR;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = _alloc_dirent(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = new_mallocfs_vnode_with_block(mode, vnode);
	if (!newnode)
		return ENOMEM;

	dir->vnode = newnode;

	if (result)
		*result = newnode;
	return 0;
}

int mallocfs_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, struct vnode **result)
{
	struct vnode *newnode;
	struct mallocfs_dirent *dir;

	if (!(vnode->mode & S_IFDIR))
		return ENOTDIR;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = _alloc_dirent(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = new_mallocfs_vnode(dev, mode);
	if (!newnode)
		return EMFILE;
	dir->vnode = newnode;

	if (result)
		*result = newnode;
	return 0;
}

int mallocfs_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	struct zone_iter iter;
	struct mallocfs_block *zone;

	// If a valid pointer isn't provided, return invalid argument
	if (!result)
		return EINVAL;

	if (!(vnode->mode & S_IFDIR))
		return ENOTDIR;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	zone_iter_start(&iter);
	while ((zone = zone_iter_next(&iter, MALLOCFS_DATA(vnode).zones))) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode && !strcmp(filename, zone->entries[i].name)) {
				*result = zone->entries[i].vnode;
				return 0;
			}
		}
	}
	return ENOENT;
}

int mallocfs_unlink(struct vnode *parent, struct vnode *vnode)
{
	struct mallocfs_dirent *dir;

	if (vnode->mode & S_IFDIR && !_is_empty_dirent(vnode))
		return ENOTEMPTY;

	dir = _find_dirent(parent, vnode);
	if (!dir)
		return ENOENT;

	dir->vnode = NULL;
	// TODO should this be moved into vfs.c
	vfs_release_vnode(vnode);
}

int mallocfs_truncate(struct vnode *vnode)
{
	if (vnode->mode & S_IFDIR)
		return EISDIR;
	zone_free_all(vnode);
	vnode->size = 0;
	return 0;
}

int mallocfs_release(struct vnode *vnode)
{
	// NOTE the refcount decrement that triggered this call should be enough to free the vnode itself
	zone_free_all(vnode);
	return 0;
}


int mallocfs_open(struct vfile *file, int flags)
{
	return 0;
}

int mallocfs_close(struct vfile *file)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_close(MALLOCFS_DATA(file->vnode).device);

	return 0;
}

int mallocfs_read(struct vfile *file, char *buf, size_t nbytes)
{
	if (file->vnode->mode & S_IFDIR)
		return EISDIR;

	if (file->vnode->mode & S_IFCHR)
		return dev_read(MALLOCFS_DATA(file->vnode).device, buf, nbytes);

	else {
		char *zone;
		short zpos;
		short zlen;
		size_t offset = 0;
		mallocfs_zone_t znum;

		if (nbytes > file->vnode->size - file->position)
			nbytes = file->vnode->size - file->position;

		znum = file->position >> MALLOCFS_LOG_BLOCK_SIZE;
		zpos = file->position & (MALLOCFS_BLOCK_SIZE - 1);

		do {
			zone = (char *) zone_lookup(file->vnode, znum, 0);
			if (!zone)
				break;

			zlen = MALLOCFS_BLOCK_SIZE - zpos;
			if (zlen > nbytes)
				zlen = nbytes;

			memcpy_s(&buf[offset], &zone[zpos], zlen);

			offset += zlen;
			nbytes -= zlen;
			znum += 1;
			zpos = 0;
		} while (nbytes > 0);

		file->position += offset;
		return offset;
	}
}

int mallocfs_write(struct vfile *file, const char *buf, size_t nbytes)
{
	if (file->vnode->mode & S_IFDIR)
		return EISDIR;

	if (file->vnode->mode & S_IFCHR)
		return dev_write(MALLOCFS_DATA(file->vnode).device, buf, nbytes);

	else {
		char *zone;
		short zpos;
		short zlen;
		int error = 0;
		size_t offset = 0;
		mallocfs_zone_t znum;

		znum = file->position >> MALLOCFS_LOG_BLOCK_SIZE;
		zpos = file->position & (MALLOCFS_BLOCK_SIZE - 1);

		do {
			zone = (char *) zone_lookup(file->vnode, znum, MFS_CREATE_ZONE);
			if (!zone) {
				error = ENOSPC;
				break;
			}

			zlen = MALLOCFS_BLOCK_SIZE - zpos;
			if (zlen > nbytes)
				zlen = nbytes;

			memcpy_s(&zone[zpos], &buf[offset], zlen);

			offset += zlen;
			nbytes -= zlen;
			znum += 1;
			zpos = 0;
		} while (nbytes > 0);

		file->position += offset;
		if (file->position > file->vnode->size)
			file->vnode->size = file->position;

		if (error)
			return error;
		return offset;
	}
}

int mallocfs_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_ioctl(MALLOCFS_DATA(file->vnode).device, request, argp);

	return -1;
}

offset_t mallocfs_seek(struct vfile *file, offset_t position, int whence)
{
	if (file->vnode->mode & S_IFCHR)
		return -1;

	else {
		switch (whence) {
		    case SEEK_SET:
			break;
		    case SEEK_CUR:
			position = file->position + position;
			break;
		    case SEEK_END:
			position = file->vnode->size + position;
			break;
		    default:
			return EINVAL;
		}

		// TODO this is a hack for now so I don't have to deal with gaps in files
		if (position > file->vnode->size)
			position = file->vnode->size;

		file->position = position;
		return file->position;
	}
}

int mallocfs_readdir(struct vfile *file, struct vdir *dir)
{
	int max;
	short zpos;
	mallocfs_zone_t znum;
	struct mallocfs_block *zone;

	if (!(file->vnode->mode & S_IFDIR))
		return ENOTDIR;

	znum = file->position >> MALLOCFS_LOG_DIRENTS;
	zpos = file->position & (MALLOCFS_DIRENTS - 1);

	while (1) {
		zone = zone_lookup(file->vnode, znum, 0);
		if (!zone)
			return 0;

		// Advance to the next valid directory entry in the block
		for (; zpos < MALLOCFS_DIRENTS - 1 && zone->entries[zpos].vnode == NULL; zpos++) { }

		if (zone->entries[zpos].vnode != NULL)
			break;

		znum++;
	}

	file->position = ((znum << MALLOCFS_LOG_DIRENTS) | zpos) + 1;

	max = MALLOCFS_MAX_FILENAME < VFS_FILENAME_MAX ? MALLOCFS_MAX_FILENAME : VFS_FILENAME_MAX;

	dir->vnode = zone->entries[zpos].vnode;
	strncpy(dir->name, zone->entries[zpos].name, max);
	dir->name[max - 1] = '\0';

	return 1;
}


static struct mallocfs_dirent *_alloc_dirent(struct vnode *vnode, const char *filename)
{
	struct mallocfs_dirent *dir;

	dir = _find_empty_dirent(vnode);
	if (!dir)
		return NULL;

	strncpy(dir->name, filename, MALLOCFS_MAX_FILENAME);
	dir->name[MALLOCFS_MAX_FILENAME - 1] = '\0';

	return dir;
}

static struct mallocfs_dirent *_find_empty_dirent(struct vnode *dir)
{
	struct mallocfs_block *zone;

	for (mallocfs_zone_t znum = 0; zone = zone_lookup(dir, znum, MFS_CREATE_ZONE); znum++) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode == NULL)
				return &zone->entries[i];
		}
	}
	return NULL;
}

static struct mallocfs_dirent *_find_dirent(struct vnode *dir, struct vnode *file)
{
	struct zone_iter iter;
	struct mallocfs_block *zone;

	zone_iter_start(&iter);
	while((zone = zone_iter_next(&iter, MALLOCFS_DATA(dir).zones))) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode == file)
				return &zone->entries[i];
		}
	}
	return NULL;
}

static short _is_empty_dirent(struct vnode *vnode)
{
	struct zone_iter iter;
	struct mallocfs_block *zone;

	zone_iter_start(&iter);
	while ((zone = zone_iter_next(&iter, MALLOCFS_DATA(vnode).zones))) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
			if (zone->entries[i].vnode && strcmp(zone->entries[i].name, ".") && strcmp(zone->entries[i].name, ".."))
				return 0;
		}
	}
	return 1;
}


