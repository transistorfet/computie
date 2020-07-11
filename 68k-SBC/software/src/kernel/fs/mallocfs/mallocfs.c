
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <kernel/vfs.h>
#include <kernel/driver.h>
 
#include "mallocfs.h"
#include "../vnode.h"

#include <stdlib.h>
#define MFS_ALLOC_BLOCK()	((struct mallocfs_block *) malloc(MALLOCFS_BLOCK_SIZE))
#define MFS_FREE_BLOCK(ptr)	free(ptr)


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
};


struct vnode *mallocfs_root;
//static struct mallocfs_dirent devices[MALLOCFS_DIRENT_MAX];

static struct mallocfs_dirent *_find_dirent(struct vnode *dir, struct vnode *file);
static struct mallocfs_dirent *_find_empty_dirent(struct vnode *vnode);
static short _is_empty_dirent(struct vnode *vnode);
static struct mallocfs_dirent *_alloc_dirent(struct vnode *vnode, const char *filename);
static struct vnode *_new_vnode_with_block(mode_t mode);

int init_mallocfs()
{
	mallocfs_root = _new_vnode_with_block(S_IFDIR | 0755);

	int error;
	struct vnode *vn;

	error = mallocfs_create(mallocfs_root, "dir", S_IFDIR | 0755, &vn);
	if (error)
		printf("Error: %d\n", error);
	else
		printf("Created dir at %x\n", vn->block);

	error = mallocfs_create(vn, "test", 0644, &vn);
	if (error)
		printf("Error: %d\n", error);
	else
		printf("Created at %x\n", vn->block);

}

int mallocfs_create(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result)
{
	struct vnode *newnode;
	struct mallocfs_dirent *dir;
	struct mallocfs_block *block;

	if (!(vnode->mode & S_IFDIR))
		return ENOTDIR;

	// The dirent wont be 'in use' until we set the vnode reference
	dir = _alloc_dirent(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = _new_vnode_with_block(mode);
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

	// The dirent wont be 'in use' until we set the vnode reference
	dir = _alloc_dirent(vnode, filename);
	if (!dir)
		return ENOSPC;

	newnode = new_vnode(dev, mode, &mallocfs_vnode_ops);
	if (!newnode)
		return EMFILE;
	dir->vnode = newnode;

	if (result)
		*result = newnode;
	return 0;
}

int mallocfs_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	// If a valid pointer isn't provided, return invalid argument
	if (!result)
		return EINVAL;

	if (!(vnode->mode & S_IFDIR))
		return ENOTDIR;

	struct mallocfs_block *block = vnode->block;
	for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
		if (block->entries[i].vnode && !strcmp(filename, block->entries[i].name)) {
			*result = block->entries[i].vnode;
			return 0;
		}
	}
	return ENOENT;
}

int mallocfs_unlink(struct vnode *parent, struct vnode *vnode)
{
	struct mallocfs_dirent *dir;

	if (vnode->mode & S_IFDIR && !_is_empty_dirent(vnode))
		// TODO you don't have an error code for dir not empty
		return -1;

	dir = _find_dirent(parent, vnode);
	if (!dir)
		return ENOENT;

	dir->vnode = NULL;
	if (vnode->block)
		free(vnode->block);
	free_vnode(vnode);
}

int mallocfs_open(struct vfile *file, int flags)
{
	return 0;
}

int mallocfs_close(struct vfile *file)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_close(file->vnode->device);

	return 0;
}

int mallocfs_read(struct vfile *file, char *buf, size_t nbytes)
{
	if (file->vnode->mode & S_IFDIR)
		return EISDIR;

	if (file->vnode->mode & S_IFCHR)
		return dev_read(file->vnode->device, buf, nbytes);

	else {
		register struct vnode *vnode = file->vnode;
		register char *block = vnode->block;

		if (nbytes > vnode->size - file->position)
			nbytes = vnode->size - file->position;
		memcpy_s(buf, &block[file->position], nbytes);
		file->position += nbytes;
		return nbytes;
	}
}

int mallocfs_write(struct vfile *file, const char *buf, size_t nbytes)
{
	if (file->vnode->mode & S_IFDIR)
		return EISDIR;

	if (file->vnode->mode & S_IFCHR)
		return dev_write(file->vnode->device, buf, nbytes);

	else {
		register struct vnode *vnode = file->vnode;
		register char *block = vnode->block;

		if (nbytes > MALLOCFS_BLOCK_SIZE - file->position)
			nbytes = MALLOCFS_BLOCK_SIZE - file->position;
		memcpy_s(&block[file->position], buf, nbytes);
		file->position += nbytes;

		if (file->position > vnode->size)
			vnode->size = file->position;

		return nbytes;
	}
}

int mallocfs_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	if (file->vnode->mode & S_IFCHR)
		return dev_ioctl(file->vnode->device, request, argp);

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
	struct mallocfs_dirent *mdir;
	struct mallocfs_block *block = file->vnode->block;

	if (!(file->vnode->mode & S_IFDIR))
		return ENOTDIR;

	// Advance to the next valid directory entry in the block
	for (; block->entries[file->position].vnode == NULL; file->position++) { }

	if (file->position >= MALLOCFS_DIRENTS)
		return 0;

	max = MALLOCFS_MAX_FILENAME < VFS_FILENAME_MAX ? MALLOCFS_MAX_FILENAME : VFS_FILENAME_MAX;

	mdir = &block->entries[file->position];
	file->position += 1;

	dir->vnode = mdir->vnode;
	strncpy(dir->name, mdir->name, max);
	dir->name[max - 1] = '\0';

	return 1;
}


static struct mallocfs_dirent *_find_dirent(struct vnode *dir, struct vnode *file)
{
	struct mallocfs_block *block = dir->block;

	for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
		if (block->entries[i].vnode == file)
			return &block->entries[i];
	}
	return NULL;
}

static struct mallocfs_dirent *_find_empty_dirent(struct vnode *vnode)
{
	struct mallocfs_block *block = vnode->block;

	for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
		if (!block->entries[i].vnode)
			return &block->entries[i];
	}
	return NULL;
}

static short _is_empty_dirent(struct vnode *vnode)
{
	struct mallocfs_block *block = vnode->block;

	for (short i = 0; i < MALLOCFS_DIRENTS; i++) {
		if (block->entries[i].vnode)
			return 0;
	}
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

static struct vnode *_new_vnode_with_block(mode_t mode)
{
	struct vnode *vnode;
	struct mallocfs_block *block;

	block = MFS_ALLOC_BLOCK();
	if (!block)
		return NULL;

	vnode = new_vnode(0, mode, &mallocfs_vnode_ops);
	if (!vnode) {
		MFS_FREE_BLOCK(block);
		return NULL;
	}

	vnode->block = block;
	if (mode & S_IFDIR) {
		for (short i = 0; i < MALLOCFS_DIRENTS; i++)
			block->entries[i].vnode = NULL;
	}

	return vnode;
}


