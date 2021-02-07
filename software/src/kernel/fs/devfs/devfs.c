
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <kernel/vfs.h>
#include <kernel/driver.h>
 
#include "devfs.h"
#include "../nop.h"

struct vfile_ops devfs_vfile_ops = {
	devfs_open,
	devfs_close,
	devfs_read,
	devfs_write,
	devfs_ioctl,
	devfs_seek,
};

struct vnode_ops devfs_vnode_ops = {
	&devfs_vfile_ops,
	NULL,
	devfs_mknod,
	devfs_lookup,
	nop_link,
	nop_unlink,
	nop_rename,
	nop_truncate,
	nop_update,
	devfs_release,
};

#define MAX_VNODES	6

struct vnode *devfs_root;
static struct vnode vnode_table[MAX_VNODES];
static struct devfs_dirent devices[DEVFS_DIRENT_MAX];


static inline struct devfs_dirent *_new_dirent();
static struct vnode *_new_vnode(device_t dev, mode_t mode, struct vnode_ops *ops);

int init_devfs()
{
	devfs_root = _new_vnode(0, 0755, &devfs_vnode_ops);

	for (char i = 0; i < DEVFS_DIRENT_MAX; i++)
		devices[i].vnode = NULL;
}

int devfs_create(struct vnode *vnode, const char *filename, mode_t mode, uid_t uid, struct vnode **result)
{

}

int devfs_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, uid_t uid, struct vnode **result)
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

	newnode = _new_vnode(dev, mode, &devfs_vnode_ops);
	if (!newnode)
		return EMFILE;

	dir->vnode = newnode;
	strncpy(dir->name, filename, DEVFS_MAX_FILENAME);
	dir->name[DEVFS_MAX_FILENAME - 1] = '\0';

	if (result)
		*result = newnode;
	return 0;
}

int devfs_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	// If a valid pointer isn't provided, return invalid argument
	if (!result)
		return EINVAL;

	// Only support a single level of vnodes below the root vnode
	if (vnode != devfs_root)
		return ENOTDIR;

	for (char i = 0; i < DEVFS_DIRENT_MAX; i++) {
		if (devices[i].vnode && !strcmp(filename, devices[i].name)) {
			if (*result)
				vfs_release_vnode(*result);
			*result = devices[i].vnode;
			return 0;
		}
	}
	return ENOENT;
}

int devfs_release(struct vnode *vnode)
{

}

int devfs_open(struct vfile *file, int flags)
{
	return 0;
}

int devfs_close(struct vfile *file)
{
	return dev_close(DEVFS_DATA(file->vnode).device);
}

int devfs_read(struct vfile *file, char *buf, size_t nbytes)
{
	return dev_read(DEVFS_DATA(file->vnode).device, buf, 0, nbytes);
}

int devfs_write(struct vfile *file, const char *buf, size_t nbytes)
{
	return dev_write(DEVFS_DATA(file->vnode).device, buf, 0, nbytes);
}

int devfs_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	return dev_ioctl(DEVFS_DATA(file->vnode).device, request, argp);
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

static struct vnode *_new_vnode(device_t dev, mode_t mode, struct vnode_ops *ops)
{
	for (char i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].refcount <= 0) {
			vfs_init_vnode(&vnode_table[i], ops, NULL, mode, 1, 0, 0, 0, dev, 0, 0, 0, 0);
			DEVFS_DATA(&vnode_table[i]).device = dev;
			return &vnode_table[i];
		}
	}
	return NULL;
}


