
#include <string.h>

#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/filedesc.h>

// TODO this will be removed
#include "mallocfs/mallocfs.h"

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


int vfs_mknod(const char *path, mode_t mode, device_t dev, struct vnode **result)
{
	int error;
	struct vnode *vnode;

	error = vfs_lookup(path, O_CREAT, &vnode);
	if (error)
		return error;

	const char *filename = path_last_component(path);
	return vnode->ops->mknod(vnode, filename, mode, dev, result);
}

int vfs_lookup(const char *path, int flags, struct vnode **result)
{
	// TODO this is temporary because we don't have mounts yet
	extern struct vnode *mallocfs_root;

	int error;
	int i = 0, j;
	char component[VFS_FILENAME_MAX];
	struct vnode *cur = mallocfs_root;

	if (!result)
		return EINVAL;

	// We are always starting from the root node, so ignore a leading slash
	if (path[0] == VFS_SEP)
		i += 1;

	while (1) {
		if (path[i] == '\0') {
			*result = cur;
			return 0;
		}

		for (j = 0; path[i] && path[i] != VFS_SEP; i++, j++)
			component[j] = path[i];
		if (path[i] == VFS_SEP)
			i += 1;
		component[j] = '\0';

		// If creating, then skip the last component lookup
		if (flags & O_CREAT && path[i] == '\0')
			continue;

		error = cur->ops->lookup(cur, component, &cur);
		if (error)
			return error;
	}

	return ENOENT;
}

int vfs_unlink(const char *path)
{
	int error;
	struct vnode *vnode;

	// TODO this might need to get the parent and pass it to unlink
	error = vfs_lookup(path, 0, &vnode);
	if (error)
		return error;

	error = vnode->ops->unlink(vnode);
	if (error)
		return error;
	return 0;
}

int vfs_open(const char *path, int flags, struct vfile **file)
{
	int error;
	struct vnode *vnode;

	if (!file)
		return EINVAL;

	error = vfs_lookup(path, flags, &vnode);
	if (error)
		return error;

	if (flags & O_CREAT) {
		const char *filename = path_last_component(path);

		// Lookup the last path component, or create a new file if an error occurs during lookup
		if (vnode->ops->lookup(vnode, filename, &vnode)) {
			error = vnode->ops->create(vnode, filename, 0755, &vnode);
			if (error)
				return error;
		}
	}



	*file = new_fileptr(vnode);
	if (!*file)
		return EMFILE;

	error = vnode->ops->fops->open(*file, flags);
	if (error)
		free_fileptr(*file);
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

int vfs_readdir(struct vfile *file, struct vdir *dir)
{
	return file->vnode->ops->fops->readdir(file, dir);
}


const char *path_last_component(const char *path)
{
	short i = strlen(path) - 1;
	for (; i >= 0; i--) {
		if (path[i] == VFS_SEP)
			break;
	}
	i += 1;
	return &path[i];
}
