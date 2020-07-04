
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <kernel/vfs.h>
#include <kernel/driver.h>
 
#include "devfs.h"
#include "../vnode.h"

struct vfile_ops devfs_vfile_ops = {
	devfs_close,
	devfs_read,
	devfs_write,
	devfs_ioctl,
	devfs_seek,
};

struct vnode_ops devfs_vnode_ops = {
	&devfs_vfile_ops,
	devfs_lookup,
	devfs_mknod,
	devfs_open,
};

#define DEVFS_MAX_FILENAME	14

struct devfs_dirent {
	struct vnode *vnode;
	char name[DEVFS_MAX_FILENAME];
};


#define DEVFS_DIRENT_MAX	8

struct vnode *devfs_root;
static struct devfs_dirent devices[DEVFS_DIRENT_MAX];
struct vnode *tty_vnode;


static inline struct devfs_dirent *_new_dirent();

int init_devfs()
{
	devfs_root = new_vnode(0, 0755, &devfs_vnode_ops);

	for (char i = 0; i < DEVFS_DIRENT_MAX; i++) {
		devices[i].vnode = NULL;
	}


	//devfs_mknod(devfs_root, "tty", S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, 0, &tty_vnode);
}

int devfs_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	char i;

	// If a valid pointer isn't provided, return invalid argument
	if (!result)
		return EINVAL;

	// Only support a single level of vnodes below the root vnode
	if (vnode != devfs_root)
		return ENOTDIR;

	for (i = 0; i < DEVFS_DIRENT_MAX; i++) {
		if (devices[i].vnode && !strcmp(filename, devices[i].name)) {
			*result = devices[i].vnode;
			return 0;
		}
	}
	return -ENOENT;
}

int devfs_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, struct vnode **result)
{
	struct vnode *newnode;
	struct devfs_dirent *dir;

	// Only support a single level of vnodes below the root vnode
	if (vnode != devfs_root)
		return ENOTDIR;

	// TODO check for name conflict

	dir = _new_dirent();
	if (!dir)
		return ENOSPC;

	newnode = new_vnode(dev, mode, &devfs_vnode_ops);
	if (!newnode)
		return EMFILE;

	dir->vnode = newnode;
	strncpy(dir->name, filename, DEVFS_MAX_FILENAME);
	dir->name[DEVFS_MAX_FILENAME - 1] = '\0';

	if (result)
		*result = newnode;
	return 0;
}

int devfs_open(struct vnode *vnode, mode_t mode, struct vfile **file)
{
	return 0;
}

int devfs_close(struct vfile *file)
{
	return dev_close(file->vnode->device);
}

int devfs_read(struct vfile *file, char *buf, size_t nbytes)
{
	return dev_read(file->vnode->device, buf, nbytes);
}

int devfs_write(struct vfile *file, const char *buf, size_t nbytes)
{
	return dev_write(file->vnode->device, buf, nbytes);
}

int devfs_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	return dev_ioctl(file->vnode->device, request, argp);
}

offset_t devfs_seek(struct vfile *file, offset_t position, int whence)
{
	return -1;
}


static inline struct devfs_dirent *_new_dirent()
{
	for (char i = 0; i < DEVFS_DIRENT_MAX; i++) {
		if (!devices[i].vnode)
			return &devices[i];
	}
	return NULL;
}
