
#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/filedesc.h>

// TODO this will be removed
#include "devfs/devfs.h"

#define VFS_MOUNT_MAX		2

//static struct vnode *vfs_root;
static struct mount mountpoints[VFS_MOUNT_MAX];

int init_vfs()
{
	//vfs_root = new_vnode(0, 0777);

	for (char i = 0; i < VFS_MOUNT_MAX; i++) {
		// TODO this isn't a proper way of determining use
		mountpoints[i].ops = NULL;
	}
}


int vfs_mount(struct mount *mp);
int vfs_umount(struct mount *mp);
struct vnode *vfs_root(struct mount *mp);
int vfs_sync(struct mount *mp);

int vfs_lookup(const char *filename, struct vnode **result)
{
	/*
	struct vnode *cur = vfs_root;

	while (cur) {
		
	}
	return -ENOENT;
	*/
	extern struct vnode *devfs_root;
	return devfs_lookup(devfs_root, filename, result);
}


int vfs_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, struct vnode **result)
{
	return vnode->ops->mknod(vnode, filename, mode, dev, result);
}

int vfs_open(struct vnode *vnode, mode_t mode, struct vfile **file)
{
	int error;

	if (!file)
		return EINVAL;

	*file = new_file(vnode);
	if (!*file)
		return EMFILE;

	error = vnode->ops->open(vnode, mode, file);
	if (error)
		free_file(*file);
	return error;
}

int vfs_close(struct vfile *file)
{
	return file->vnode->ops->fops->close(file);
}

int vfs_read(struct vfile *file, char *buffer, size_t size)
{
	return file->vnode->ops->fops->read(file, buffer, size);
}

int vfs_write(struct vfile *file, const char *buffer, size_t size)
{
	return file->vnode->ops->fops->write(file, buffer, size);
}

int vfs_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	return file->vnode->ops->fops->ioctl(file, request, argp);
}

offset_t vfs_seek(struct vfile *file, offset_t position, int whence)
{
	return file->vnode->ops->fops->seek(file, position, whence);
}

