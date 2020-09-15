
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
#include "dir.h"



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
	mallocfs_rename,
	mallocfs_truncate,
	mallocfs_update,
	mallocfs_release,
};

struct mount_ops mallocfs_mount_ops = {
	mallocfs_mount,
	mallocfs_unmount,
	mallocfs_sync,
};


struct mount mallocfs_root;
	

int init_mallocfs()
{
	init_mallocfs_alloc();

	// TODO this would be moved elsewhere
	struct mount *mp;
	vfs_mount(NULL, "/", NULL, &mallocfs_mount_ops, SU_UID, &mp);
}

int mallocfs_mount(struct mount *mp, struct vnode *parent)
{
	mp->root_node = new_mallocfs_vnode(0, S_IFDIR | 0755);
	dir_setup(mp->root_node, parent);
	return 0;
}

int mallocfs_unmount(struct mount *mp)
{
	// TODO implement
	return 0;
}

int mallocfs_sync(struct mount *mp)
{
	// NOTE nothing to sync
	return 0;
}

int mallocfs_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result)
{
	struct vnode *newnode;
	struct mallocfs_dirent *dir;
	struct mallocfs_block *block;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = dir_alloc_entry(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = new_mallocfs_vnode(0, mode);
	if (!newnode)
		return ENOMEM;

	if (mode & S_IFDIR && !dir_setup(newnode, vnode))
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

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = dir_alloc_entry(vnode, filename);
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
	struct mallocfs_dirent *entry;

	// If a valid pointer isn't provided, return invalid argument
	if (!result)
		return EINVAL;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	entry = dir_find_entry_by_name(vnode, filename, MFS_LOOKUP_ZONE);
	if (!entry)
		return ENOENT;

	*result = entry->vnode;
	return 0;
}

int mallocfs_unlink(struct vnode *parent, struct vnode *vnode)
{
	struct mallocfs_dirent *dir;

	if (vnode->mode & S_IFDIR && !dir_is_empty(vnode))
		return ENOTEMPTY;

	dir = dir_find_entry_by_vnode(parent, vnode, MFS_LOOKUP_ZONE);
	if (!dir)
		return ENOENT;

	dir->vnode = NULL;
	vfs_release_vnode(vnode);
}

int mallocfs_rename(struct vnode *vnode, struct vnode *oldparent, const char *oldname, struct vnode *newparent, const char *newname)
{
	struct mallocfs_dirent *olddir, *newdir;

	newdir = dir_find_entry_by_name(newparent, newname, MFS_LOOKUP_ZONE);
	if (newdir) {
		// TODO delete existing file instead of error??
		return EEXIST;
	}
	else {
		newdir = dir_alloc_entry(newparent, newname);
		if (!newdir)
			return ENOSPC;
	}

	olddir = dir_find_entry_by_name(oldparent, oldname, MFS_LOOKUP_ZONE);
	if (!olddir)
		return ENOENT;

	newdir->vnode = vnode;
	olddir->vnode = NULL;
	return 0;
}

int mallocfs_truncate(struct vnode *vnode)
{
	if (vnode->mode & S_IFDIR)
		return EISDIR;
	zone_free_all(vnode);
	vnode->size = 0;
	return 0;
}

int mallocfs_update(struct vnode *vnode)
{
	// NOTE since it's entirely in memory, we don't need to commit the change
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
	if (file->vnode->mode & S_IFCHR)
		return dev_open(MALLOCFS_DATA(file->vnode).device, flags);
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
			zone = (char *) zone_lookup(file->vnode, znum, MFS_LOOKUP_ZONE);
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
		zone = zone_lookup(file->vnode, znum, MFS_LOOKUP_ZONE);
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


