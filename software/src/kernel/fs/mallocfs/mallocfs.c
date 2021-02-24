
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
	nop_poll,
	mallocfs_seek,
	mallocfs_readdir,
};

struct vnode_ops mallocfs_vnode_ops = {
	&mallocfs_vfile_ops,
	mallocfs_create,
	mallocfs_mknod,
	mallocfs_lookup,
	mallocfs_link,
	mallocfs_unlink,
	mallocfs_rename,
	mallocfs_truncate,
	mallocfs_update,
	mallocfs_release,
};

struct mount_ops mallocfs_mount_ops = {
	"mallocfs",
	mallocfs_init,
	mallocfs_mount,
	mallocfs_unmount,
	mallocfs_sync,
};


struct mount mallocfs_root;
	

int mallocfs_init()
{
	init_mallocfs_alloc();
}

int mallocfs_mount(struct mount *mp, device_t dev, struct vnode *parent)
{
	mp->root_node = new_mallocfs_vnode(mp, 0, S_IFDIR | 0755, SU_UID);
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

int mallocfs_create(struct vnode *vnode, const char *filename, mode_t mode, uid_t uid, struct vnode **result)
{
	struct vnode *newnode;
	struct mallocfs_dirent *dir;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = dir_alloc_entry(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = new_mallocfs_vnode(vnode->mp, 0, mode, uid);
	if (!newnode)
		return ENOMEM;

	if (S_ISDIR(mode) && !dir_setup(newnode, vnode)) {
		vfs_release_vnode(newnode);
		return ENOMEM;
	}

	dir->vnode = newnode;

	*result = newnode;
	return 0;
}

int mallocfs_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, uid_t uid, struct vnode **result)
{
	struct vnode *newnode;
	struct mallocfs_dirent *dir;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = dir_alloc_entry(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = new_mallocfs_vnode(vnode->mp, dev, mode, uid);
	if (!newnode)
		return EMFILE;

	dir->vnode = newnode;

	*result = newnode;
	return 0;
}

int mallocfs_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	struct mallocfs_dirent *entry;

	if (strlen(filename) > MALLOCFS_MAX_FILENAME)
		return ENAMETOOLONG;

	entry = dir_find_entry_by_name(vnode, filename, MFS_LOOKUP_ZONE);
	if (!entry)
		return ENOENT;

	if (*result)
		vfs_release_vnode(*result);
	*result = vfs_clone_vnode(entry->vnode);
	return 0;
}

int mallocfs_link(struct vnode *oldvnode, struct vnode *newparent, const char *filename)
{
	// TODO implement
}

int mallocfs_unlink(struct vnode *parent, struct vnode *vnode, const char *filename)
{
	struct mallocfs_dirent *dir;

	if (S_ISDIR(vnode->mode) && !dir_is_empty(vnode))
		return ENOTEMPTY;

	dir = dir_find_entry_by_name(parent, filename, MFS_LOOKUP_ZONE);
	if (!dir)
		return ENOENT;

	dir->vnode = NULL;
	return 0;
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
	vfs_update_time(oldparent, MTIME);
	vfs_update_time(newparent, MTIME);
	return 0;
}

int mallocfs_truncate(struct vnode *vnode)
{
	if (S_ISDIR(vnode->mode))
		return EISDIR;
	zone_free_all(vnode);
	vnode->size = 0;
	vfs_update_time(vnode, MTIME);
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
	return 0;
}

int mallocfs_close(struct vfile *file)
{
	return 0;
}

int mallocfs_read(struct vfile *file, char *buf, size_t nbytes)
{
	if (S_ISDIR(file->vnode->mode))
		return EISDIR;

	char *zone;
	short zpos;
	short zlen;
	size_t offset = 0;
	zone_t znum;

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

		memcpy(&buf[offset], &zone[zpos], zlen);

		offset += zlen;
		nbytes -= zlen;
		znum += 1;
		zpos = 0;
	} while (nbytes > 0);

	file->position += offset;
	return offset;
}

int mallocfs_write(struct vfile *file, const char *buf, size_t nbytes)
{
	if (S_ISDIR(file->vnode->mode))
		return EISDIR;

	char *zone;
	short zpos;
	short zlen;
	int error = 0;
	size_t offset = 0;
	zone_t znum;

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

		memcpy(&zone[zpos], &buf[offset], zlen);

		offset += zlen;
		nbytes -= zlen;
		znum += 1;
		zpos = 0;
	} while (nbytes > 0);

	file->position += offset;
	if (file->position > file->vnode->size)
		file->vnode->size = file->position;

	if (offset)
		vfs_update_time(file->vnode, MTIME);

	if (error)
		return error;
	return offset;
}

int mallocfs_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid)
{
	return -1;
}

offset_t mallocfs_seek(struct vfile *file, offset_t position, int whence)
{
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

int mallocfs_readdir(struct vfile *file, struct dirent *dir)
{
	int max;
	short zpos;
	zone_t znum;
	struct mallocfs_block *zone;

	if (!S_ISDIR(file->vnode->mode))
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

	dir->d_ino = 0;	// TODO this should be something... zone->entries[zpos].vnode;
	strncpy(dir->d_name, zone->entries[zpos].name, max);
	dir->d_name[max - 1] = '\0';

	return 1;
}


