
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/driver.h>
 
#include "minix.h"
#include "bitmaps.h"
#include "inodes.h"
#include "vnodes.h"
#include "zones.h"
#include "super.h"
#include "dir.h"

#include "../bufcache.h"


struct vfile_ops minix_vfile_ops = {
	minix_open,
	minix_close,
	minix_read,
	minix_write,
	minix_ioctl,
	minix_seek,
	minix_readdir,
};

struct vnode_ops minix_vnode_ops = {
	&minix_vfile_ops,
	minix_create,
	minix_mknod,
	minix_lookup,
	minix_unlink,
	minix_rename,
	minix_truncate,
	minix_update,
	minix_release,
};

struct mount_ops minix_mount_ops = {
	minix_init,
	minix_mount,
	minix_unmount,
	minix_sync,
};


int minix_init()
{
	init_minix_vnodes();
	return 0;
}


int minix_mount(struct mount *mp, device_t dev, struct vnode *parent)
{
	struct minix_super *super = load_superblock(dev);
	if (!super)
		return -1;
	mp->dev = dev;
	mp->super = super;

	struct vnode *root = get_vnode(mp, 1);
	if (!root) {
		free_superblock((struct minix_super *) mp->super);
		return ENOMEM;
	}

	mp->root_node = root;

	return 0;
}

int minix_unmount(struct mount *mp)
{
	free_superblock((struct minix_super *) mp->super);
	vfs_release_vnode(mp->root_node);
	mp->root_node = NULL;
	return 0;
}

int minix_sync(struct mount *mp)
{
	sync_vnodes();
	sync_bufcache();
	return 0;
}

int minix_create(struct vnode *vnode, const char *filename, mode_t mode, uid_t uid, struct vnode **result)
{
	struct buf *buf;
	struct vnode *newnode;
	struct minix_v1_dirent *dir;

	if (strlen(filename) > MINIX_V1_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = dir_alloc_entry(vnode, filename, &buf);
	if (!dir)
		return ENOSPC;

	// TODO this needs the uid/gid from vfs
	newnode = (struct vnode *) alloc_vnode(vnode->mp, mode, uid, 0, 0);
	if (!newnode) {
		release_block(buf, 0);
		return ENOMEM;
	}

	if (mode & S_IFDIR && !dir_setup(newnode, vnode)) {
		vfs_release_vnode(newnode);
		release_block(buf, 0);
		return ENOMEM;
	}

	dir->inode = MINIX_DATA(newnode).ino;
	release_block(buf, BCF_DIRTY);

	*result = newnode;
	return 0;
}

int minix_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, uid_t uid, struct vnode **result)
{
	struct buf *buf;
	struct vnode *newnode;
	struct minix_v1_dirent *dir;

	if (strlen(filename) > MINIX_V1_MAX_FILENAME)
		return ENAMETOOLONG;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = dir_alloc_entry(vnode, filename, &buf);
	if (!dir)
		return ENOSPC;

	// TODO this needs the uid/gid from vfs
	newnode = (struct vnode *) alloc_vnode(vnode->mp, mode, uid, 0, dev);
	if (!newnode) {
		release_block(buf, 0);
		return EMFILE;
	}

	dir->inode = MINIX_DATA(newnode).ino;
	release_block(buf, BCF_DIRTY);

	*result = newnode;
	return 0;
}

int minix_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	struct buf *buf;
	struct minix_v1_dirent *entry;

	if (strlen(filename) > MINIX_V1_MAX_FILENAME)
		return ENAMETOOLONG;

	entry = dir_find_entry_by_name(vnode, filename, MFS_LOOKUP_ZONE, &buf);
	if (!entry)
		return ENOENT;

	if (*result)
		vfs_release_vnode(*result);
	*result = (struct vnode *) get_vnode(vnode->mp, entry->inode);
	release_block(buf, 0);
	return 0;
}

int minix_unlink(struct vnode *parent, struct vnode *vnode)
{
	struct buf *buf;
	struct minix_v1_dirent *dir;

	if (vnode->mode & S_IFDIR && !dir_is_empty(vnode))
		return ENOTEMPTY;

	dir = dir_find_entry_by_inode(parent, MINIX_DATA(vnode).ino, MFS_LOOKUP_ZONE, &buf);
	if (!dir)
		return ENOENT;

	dir->inode = 0;
	release_block(buf, BCF_DIRTY);
	zone_free_all(vnode);
	free_vnode(vnode);
	return 0;
}

int minix_rename(struct vnode *vnode, struct vnode *oldparent, const char *oldname, struct vnode *newparent, const char *newname)
{
	struct buf *oldbuf, *newbuf;
	struct minix_v1_dirent *olddir, *newdir;

	newdir = dir_find_entry_by_name(newparent, newname, MFS_LOOKUP_ZONE, &newbuf);
	if (newdir) {
		// TODO delete existing file instead of error??
		release_block(newbuf, 0);
		return EEXIST;
	}
	else {
		newdir = dir_alloc_entry(newparent, newname, &newbuf);
		if (!newdir)
			return ENOSPC;
	}

	olddir = dir_find_entry_by_name(oldparent, oldname, MFS_LOOKUP_ZONE, &oldbuf);
	if (!olddir) {
		release_block(newbuf, 0);
		return ENOENT;
	}

	newdir->inode = MINIX_DATA(vnode).ino;
	olddir->inode = 0;
	release_block(newbuf, BCF_DIRTY);
	release_block(oldbuf, BCF_DIRTY);
	return 0;
}

int minix_truncate(struct vnode *vnode)
{
	if (vnode->mode & S_IFDIR)
		return EISDIR;
	zone_free_all(vnode);
	vnode->size = 0;
	mark_vnode_dirty(vnode);
	return 0;
}

int minix_update(struct vnode *vnode)
{
	write_inode(vnode, MINIX_DATA(vnode).ino);
	return 0;
}

int minix_release(struct vnode *vnode)
{
	if (MINIX_DATA(vnode).ino == 0)
		remove_vnode(vnode);
	else if (vnode->bits & VBF_DIRTY)
		write_inode(vnode, MINIX_DATA(vnode).ino);
	return 0;
}


int minix_open(struct vfile *file, int flags)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_open(MINIX_DATA(file->vnode).zones[0], flags);
	return 0;
}

int minix_close(struct vfile *file)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_close(MINIX_DATA(file->vnode).zones[0]);
	return 0;
}

int minix_read(struct vfile *file, char *buffer, size_t nbytes)
{
	if (file->vnode->mode & S_IFDIR)
		return EISDIR;

	if (file->vnode->mode & S_IFCHR)
		return dev_read(MINIX_DATA(file->vnode).zones[0], buffer, 0, nbytes);

	else {
		short zpos;
		short zlen;
		size_t offset = 0;
		zone_t znum;
		zone_t zone;
		struct buf *buf;

		if (nbytes > file->vnode->size - file->position)
			nbytes = file->vnode->size - file->position;

		znum = file->position >> MINIX_V1_LOG_ZONE_SIZE;
		zpos = file->position & (MINIX_V1_ZONE_SIZE - 1);

		do {
			zone = zone_lookup(file->vnode, znum, MFS_LOOKUP_ZONE);
			if (!zone)
				break;
			buf = get_block(file->vnode->mp->dev, zone);
			if (!buf)
				break;

			zlen = MINIX_V1_ZONE_SIZE - zpos;
			if (zlen > nbytes)
				zlen = nbytes;

			memcpy_s(&buffer[offset], &(((char *) buf->block)[zpos]), zlen);
			release_block(buf, 0);

			offset += zlen;
			nbytes -= zlen;
			znum += 1;
			zpos = 0;
		} while (nbytes > 0);

		file->position += offset;
		return offset;
	}
}

int minix_write(struct vfile *file, const char *buffer, size_t nbytes)
{
	if (file->vnode->mode & S_IFDIR)
		return EISDIR;

	if (file->vnode->mode & S_IFCHR)
		return dev_write(MINIX_DATA(file->vnode).zones[0], buffer, 0, nbytes);

	else {
		short zpos;
		short zlen;
		int error = 0;
		size_t offset = 0;
		zone_t znum;
		zone_t zone;
		struct buf *buf;

		znum = file->position >> MINIX_V1_LOG_ZONE_SIZE;
		zpos = file->position & (MINIX_V1_ZONE_SIZE - 1);

		do {
			zone = zone_lookup(file->vnode, znum, MFS_CREATE_ZONE);
			if (!zone) {
				error = ENOSPC;
				break;
			}
			buf = get_block(file->vnode->mp->dev, zone);
			if (!buf) {
				error = EIO;
				break;
			}

			zlen = MINIX_V1_ZONE_SIZE - zpos;
			if (zlen > nbytes)
				zlen = nbytes;

			memcpy_s(&(((char *) buf->block)[zpos]), &buffer[offset], zlen);
			release_block(buf, BCF_DIRTY);

			offset += zlen;
			nbytes -= zlen;
			znum += 1;
			zpos = 0;
		} while (nbytes > 0);

		file->position += offset;
		if (file->position > file->vnode->size) {
			file->vnode->size = file->position;
			mark_vnode_dirty(file->vnode);
		}

		if (error)
			return error;
		return offset;
	}
}

int minix_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_ioctl(MINIX_DATA(file->vnode).zones[0], request, argp);
	return -1;
}

offset_t minix_seek(struct vfile *file, offset_t position, int whence)
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

int minix_readdir(struct vfile *file, struct dirent *dir)
{
	int max;
	short zpos;
	zone_t znum;
	zone_t zone;
	struct buf *buf;
	struct minix_v1_dirent *entries;

	if (!(file->vnode->mode & S_IFDIR))
		return ENOTDIR;

	znum = file->position >> MINIX_V1_LOG_DIRENTS_PER_ZONE;
	zpos = file->position & (MINIX_V1_DIRENTS_PER_ZONE - 1);

	while (1) {
		zone = zone_lookup(file->vnode, znum, MFS_LOOKUP_ZONE);
		if (!zone)
			return 0;
		buf = get_block(file->vnode->mp->dev, zone);
		if (!buf)
			return EIO;
		entries = (struct minix_v1_dirent *) buf->block;

		// Advance to the next valid directory entry in the block
		for (; zpos < MINIX_V1_DIRENTS_PER_ZONE - 1 && entries[zpos].inode == 0; zpos++) { }

		if (entries[zpos].inode != 0)
			break;

		release_block(buf, 0);
		znum++;
	}

	file->position = ((znum << MINIX_V1_LOG_DIRENTS_PER_ZONE) | zpos) + 1;

	max = MINIX_V1_MAX_FILENAME < VFS_FILENAME_MAX ? MINIX_V1_MAX_FILENAME : VFS_FILENAME_MAX;

	dir->ino = entries[zpos].inode;
	strncpy(dir->name, entries[zpos].filename, max);
	dir->name[max - 1] = '\0';
	release_block(buf, BCF_DIRTY);

	return 1;
}


