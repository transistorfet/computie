
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>

#include "fileptr.h"
#include "bufcache.h"

#include "../access.h"

// TODO this will be removed when you add fs mounting
#include "mallocfs/mallocfs.h"

#define VFS_MOUNT_MAX		2

static struct mount *root_fs;
static struct mount mountpoints[VFS_MOUNT_MAX];

static struct mount *_find_mount_by_vnode(struct vnode *mount)
{
	root_fs = NULL;

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].mount_node == mount)
			return &mountpoints[i];
	}
	return NULL;
}


int init_vfs()
{
	//vfs_root = new_vnode(0, 0777);

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		mountpoints[i].ops = NULL;
	}

	init_bufcache();
	init_fileptr_table();
}


int vfs_mount(struct vnode *cwd, const char *path, struct vnode *dev, struct mount_ops *ops, uid_t uid, struct mount **result)
{
	int error;
	struct vnode *vnode;

	if (uid != SU_UID)
		return EPERM;

	if (!root_fs)
		vnode = NULL;
	else {
		error = vfs_lookup(cwd, path, VLOOKUP_NORMAL, uid, &vnode);
		if (error)
			return error;
	}

	if (vnode && (vnode->bits & VBF_MOUNTED))
		return EBUSY;

	// TODO make sure not already mounted

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].ops == NULL) {
			mountpoints[i].ops = ops;
			mountpoints[i].mount_node = vnode;
			mountpoints[i].root_node = NULL;

			error = ops->mount(&mountpoints[i], vnode);
			if (error) {
				mountpoints[i].ops = NULL;
				return error;
			}

			if (vnode) {
				vfs_make_vnode_ref(vnode);
				vnode->bits |= VBF_MOUNTED;
			}
			else
				root_fs = &mountpoints[i];

			if (*result)
				*result = &mountpoints[i];
			return 0;
		}
	}
	return ENOMEM;
}

int vfs_unmount(struct mount *mp, uid_t uid)
{
	int error;

	if (uid != SU_UID)
		return EPERM;

	error = mp->ops->unmount(mp);
	if (error)
		return error;

	if (mp->mount_node) {
		mp->mount_node->bits &= ~VBF_MOUNTED;
		vfs_release_vnode(mp->mount_node);
	}
	else
		root_fs = NULL;
	mp->ops = NULL;
	return 0;
}

int vfs_sync(struct mount *mp)
{
	return mp->ops->sync(mp);
}


int vfs_lookup(struct vnode *cwd, const char *path, int flags, uid_t uid, struct vnode **result)
{
	// TODO this is temporary because we don't have mounts yet
	extern struct mount mallocfs_root;

	int error;
	int i = 0, j;
	struct mount *mp;
	struct vnode *cur;
	char component[VFS_FILENAME_MAX];

	if (!result)
		return EINVAL;

	cur = cwd ? cwd : root_fs->root_node;
	// We are always starting from the root node, so ignore a leading slash
	if (path[0] == VFS_SEP) {
		cur = root_fs->root_node;
		i += 1;
	}

	while (1) {
		if (cur->bits & VBF_MOUNTED) {
			mp = _find_mount_by_vnode(cur);
			if (!mp)
				return ENXIO;
			cur = mp->root_node;
		}

		if (path[i] == '\0') {
			*result = cur;
			return 0;
		}

		// TODO this seems to be wrong according to how linux behaves...
		if (!verify_mode_access(uid, R_OK, cur->uid, cur->gid, cur->mode))
			return EPERM;

		for (j = 0; path[i] && path[i] != VFS_SEP; i++, j++)
			component[j] = path[i];
		if (path[i] == VFS_SEP)
			i += 1;
		component[j] = '\0';

		if (!(cur->mode & S_IFDIR))
			return ENOTDIR;

		// If creating, then skip the last component lookup
		if (flags & VLOOKUP_PARENT_OF && path[i] == '\0')
			continue;

		error = cur->ops->lookup(cur, component, &cur);
		if (error)
			return error;
	}

	return ENOENT;
}

int vfs_access(struct vnode *cwd, const char *path, int mode, uid_t uid)
{
	int error;
	struct vnode *vnode;

	error = vfs_lookup(cwd, path, VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	if (mode && !verify_mode_access(uid, mode, vnode->uid, vnode->gid, vnode->mode))
		return EPERM;
	return 0;
}


int vfs_chmod(struct vnode *cwd, const char *path, int mode, uid_t uid)
{
	int error;
	struct vnode *vnode;

	error = vfs_lookup(cwd, path, VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode))
		return EPERM;

	vnode->mode = mode & 07777;
	return vnode->ops->update(vnode);
}

int vfs_chown(struct vnode *cwd, const char *path, uid_t owner, gid_t group, uid_t uid)
{
	int error;
	struct vnode *vnode;

	error = vfs_lookup(cwd, path, VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode))
		return EPERM;
	vnode->uid = owner;
	vnode->gid = group;
	return vnode->ops->update(vnode);
}

int vfs_mknod(struct vnode *cwd, const char *path, mode_t mode, device_t dev, uid_t uid, struct vnode **result)
{
	int error;
	struct vnode *tmp;
	struct vnode *vnode;

	error = vfs_lookup(cwd, path, VLOOKUP_PARENT_OF, uid, &vnode);
	if (error)
		return error;

	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode))
		return EPERM;

	const char *filename = path_last_component(path);
	error = vnode->ops->lookup(vnode, filename, &tmp);
	if (error == 0)
		return EEXIST;
	else if (error != ENOENT)
		return error;

	return vnode->ops->mknod(vnode, filename, mode, dev, result);
}

int vfs_unlink(struct vnode *cwd, const char *path, uid_t uid)
{
	int error;
	const char *filename;
	struct vnode *vnode, *parent;

	error = vfs_lookup(cwd, path, VLOOKUP_PARENT_OF, uid, &parent);
	if (error)
		return error;

	// Verify that parent directory is writable
	if (!verify_mode_access(uid, W_OK, parent->uid, parent->gid, parent->mode))
		return EACCES;

	filename = path_last_component(path);
	error = parent->ops->lookup(parent, filename, &vnode);
	if (error)
		return error;

	// Verify that the file we're trying to delete is writable
	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode))
		return EPERM;

	error = vnode->ops->unlink(parent, vnode);
	if (error)
		return error;
	return 0;
}

static inline int _rename_find_parent(struct vnode *cwd, const char *path, uid_t uid, struct vnode **result)
{
	int error;
	struct vnode *vnode;

	// Look up old parent
	error = vfs_lookup(cwd, path, VLOOKUP_PARENT_OF, uid, &vnode);
	if (error)
		return error;

	// Verify that the parent directory of the old location is writable and searchable
	if (!verify_mode_access(uid, W_OK | X_OK, vnode->uid, vnode->gid, vnode->mode))
		return EACCES;

	*result = vnode;
	return 0;
}

int vfs_rename(struct vnode *cwd, const char *oldpath, const char *newpath, uid_t uid)
{
	int error;
	const char *oldname, *newname;
	struct vnode *vnode, *oldparent, *newparent;

	oldname = path_last_component(oldpath);
	if (*oldname == '\0' || !strcmp(oldname, ".") || !strcmp(oldname, ".."))
		return EINVAL;

	newname = path_last_component(newpath);
	if (*newname == '\0' || !strcmp(newname, ".") || !strcmp(newname, ".."))
		return EINVAL;

	error = _rename_find_parent(cwd, oldpath, uid, &oldparent);
	if (error)
		return error;

	error = _rename_find_parent(cwd, newpath, uid, &newparent);
	if (error)
		return error;

	// TODO verify that both locations are on the same device

	error = oldparent->ops->lookup(oldparent, oldname, &vnode);
	if (error)
		return error;

	return vnode->ops->rename(vnode, oldparent, oldname, newparent, newname);
}

int vfs_open(struct vnode *cwd, const char *path, int flags, mode_t mode, uid_t uid, struct vfile **file)
{
	int error;
	struct vnode *vnode;
	mode_t required_mode = 0;

	if (!file)
		return EINVAL;

	error = vfs_lookup(cwd, path, (flags & O_CREAT) ? VLOOKUP_PARENT_OF : VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	if (flags & O_CREAT) {
		const char *filename = path_last_component(path);
		if (!path_valid_component(filename))
			return EINVAL;

		// Lookup the last path component, or create a new file if an error occurs during lookup
		if (vnode->ops->lookup(vnode, filename, &vnode)) {
			// Verify that parent directory is writable
			if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode))
				return EPERM;

			error = vnode->ops->create(vnode, filename, mode, &vnode);
			if (error)
				return error;
		}
		else {
			// TODO this needs to be refactored
			vfs_make_vnode_ref(vnode);
		}
	}
	else {
		vfs_make_vnode_ref(vnode);
	}

	// Verify the permissions before proceeding
	if ((flags & O_ACCMODE) != O_WRONLY)
		required_mode |= R_OK;
	if ((flags & O_ACCMODE) != O_RDONLY)
		required_mode |= W_OK;
	if (!verify_mode_access(uid, required_mode, vnode->uid, vnode->gid, vnode->mode)) {
		vfs_release_vnode(vnode);
		return EPERM;
	}

	*file = new_fileptr(vnode, flags);
	if (!*file)
		return EMFILE;

	if (flags & O_TRUNC && !(vnode->mode & S_IFDIR))
		vnode->ops->truncate(vnode);

	if (flags & O_APPEND && !(vnode->mode & S_IFDIR))
		(*file)->position = (*file)->vnode->size;

	error = vnode->ops->fops->open(*file, flags);
	if (error)
		free_fileptr(*file);
	return error;
}

int vfs_close(struct vfile *file)
{
	int error;

	error = file->vnode->ops->fops->close(file);
	free_fileptr(file);
	return error;
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

struct vfile *vfs_duplicate_fileptr(struct vfile *file)
{
	return dup_fileptr(file);
}

int vfs_release_vnode(struct vnode *vnode)
{
	vnode->refcount--;
	if (vnode->refcount < 0)
		printk("Error: double free of vnode, %x\n", vnode);
	else if (vnode->refcount == 0)
		return vnode->ops->release(vnode);
	return 0;
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

int path_valid_component(const char *path)
{
	for (; *path; path++) {
		if (*path <= 0x20 || *path == '/')
			return 0;
	}
	return 1;
}

