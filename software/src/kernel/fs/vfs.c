
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>

#include "access.h"
#include "device.h"
#include "fileptr.h"
#include "bufcache.h"


#define VFS_MOUNT_MAX		4

static struct mount *root_fs;
static struct mount mountpoints[VFS_MOUNT_MAX];

static struct mount *_find_mount_by_vnode(struct vnode *mount)
{
	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].mount_node == mount)
			return &mountpoints[i];
	}
	return NULL;
}


int init_vfs()
{
	root_fs = NULL;
	//vfs_root = new_vnode(0, 0777);

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		mountpoints[i].dev = 0;
	}

	init_bufcache();
	init_fileptr_table();
}


int vfs_mount(struct vnode *cwd, const char *path, device_t dev, struct mount_ops *ops, uid_t uid)
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

	if (vnode && (vnode->bits & VBF_MOUNTED)) {
		vfs_release_vnode(vnode);
		return EBUSY;
	}

	// TODO make sure not already mounted

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].dev == 0) {
			mountpoints[i].ops = ops;
			mountpoints[i].mount_node = vnode;
			mountpoints[i].root_node = NULL;
			mountpoints[i].dev = dev;

			error = ops->mount(&mountpoints[i], dev, vnode);
			if (error) {
				mountpoints[i].dev = 0;
				vfs_release_vnode(vnode);
				return error;
			}

			if (vnode)
				vnode->bits |= VBF_MOUNTED;
			else
				root_fs = &mountpoints[i];
			return 0;
		}
	}
	return ENOMEM;
}

int vfs_unmount(device_t dev, uid_t uid)
{
	int error;
	struct mount *mp = NULL;

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].dev == dev) {
			mp = &mountpoints[i];
			break;
		}
	}

	if (!mp)
		return ENODEV;

	if (uid != SU_UID)
		return EPERM;

	error = mp->ops->sync(mp);
	if (error)
		return error;

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

int vfs_sync(device_t dev)
{
	int error = 0;

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (!dev || mountpoints[i].dev == dev) {
			error = mountpoints[i].ops->sync(&mountpoints[i]);
			if (error)
				return error;
		}
	}
}


int vfs_lookup(struct vnode *cwd, const char *path, int flags, uid_t uid, struct vnode **result)
{
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

	vfs_clone_vnode(cur);
	while (1) {
		// If a filesystem is mounted on this node, replace the node with the mounted fs root node
		if (cur->bits & VBF_MOUNTED) {
			mp = _find_mount_by_vnode(cur);
			if (!mp) {
				vfs_release_vnode(cur);
				return ENXIO;
			}
			//if (!mp)
			//	return vfs_return(ENXIO, cur);
			cur = mp->root_node;
		}

		// Return successfully with the result
		if (path[i] == '\0') {
			*result = cur;
			return 0;
		}

		// This is not the last component, so it must be directory
		if (!(cur->mode & S_IFDIR)) {
			vfs_release_vnode(cur);
			return ENOTDIR;
		}

		// TODO this seems to be wrong according to how linux behaves...
		if (!verify_mode_access(uid, R_OK, cur->uid, cur->gid, cur->mode)) {
			vfs_release_vnode(cur);
			return EPERM;
		}

		for (j = 0; j < VFS_FILENAME_MAX - 1 && path[i] && path[i] != VFS_SEP; i++, j++)
			component[j] = path[i];
		if (path[i] == VFS_SEP)
			i += 1;
		component[j] = '\0';

		if (j >= VFS_FILENAME_MAX) {
			vfs_release_vnode(cur);
			return ENAMETOOLONG;
		}

		// If creating, then skip the last component lookup
		if (flags & VLOOKUP_PARENT_OF && path[i] == '\0')
			continue;

		// Call the fs-specific lookup to get the referenced node
		error = cur->ops->lookup(cur, component, &cur);
		if (error) {
			vfs_release_vnode(cur);
			return error;
		}
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
		error = EPERM;
	else
		error = 0;
	vfs_release_vnode(vnode);
	return error;
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
	error = vnode->ops->update(vnode);
	vfs_release_vnode(vnode);
	return error;
}

int vfs_chown(struct vnode *cwd, const char *path, uid_t owner, gid_t group, uid_t uid)
{
	int error;
	struct vnode *vnode;

	error = vfs_lookup(cwd, path, VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode)) {
		vfs_release_vnode(vnode);
		return EPERM;
	}
	vnode->uid = owner;
	vnode->gid = group;
	error = vnode->ops->update(vnode);
	vfs_release_vnode(vnode);
	return error;
}

int vfs_mknod(struct vnode *cwd, const char *path, mode_t mode, device_t dev, uid_t uid, struct vnode **result)
{
	int error;
	struct vnode *vnode;
	struct vnode *tmp = NULL;

	error = vfs_lookup(cwd, path, VLOOKUP_PARENT_OF, uid, &vnode);
	if (error)
		return error;

	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode)) {
		vfs_release_vnode(vnode);
		return EPERM;
	}

	const char *filename = path_last_component(path);
	error = vnode->ops->lookup(vnode, filename, &tmp);
	vfs_release_vnode(tmp);
	if (error == 0) {
		vfs_release_vnode(vnode);
		return EEXIST;
	}
	else if (error != ENOENT) {
		vfs_release_vnode(vnode);
		return error;
	}

	error = vnode->ops->mknod(vnode, filename, mode, dev, uid, result);
	vfs_release_vnode(vnode);
	return error;
}

int vfs_unlink(struct vnode *cwd, const char *path, uid_t uid)
{
	int error;
	const char *filename;
	struct vnode *parent;
	struct vnode *vnode = NULL;

	error = vfs_lookup(cwd, path, VLOOKUP_PARENT_OF, uid, &parent);
	if (error)
		return error;

	// Verify that parent directory is writable
	if (!verify_mode_access(uid, W_OK, parent->uid, parent->gid, parent->mode)) {
		vfs_release_vnode(parent);
		return EACCES;
	}

	filename = path_last_component(path);
	error = parent->ops->lookup(parent, filename, &vnode);
	if (error) {
		vfs_release_vnode(parent);
		return error;
	}

	// Verify that the file we're trying to delete is writable
	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode)) {
		vfs_release_vnode(parent);
		vfs_release_vnode(vnode);
		return EPERM;
	}

	// unlink takes ownership of vnode, but not parent.  If the file is deleted, unlink must call vfs_release_node
	error = vnode->ops->unlink(parent, vnode);
	vfs_release_vnode(parent);
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
	if (!verify_mode_access(uid, W_OK | X_OK, vnode->uid, vnode->gid, vnode->mode)) {
		vfs_release_vnode(vnode);
		return EACCES;
	}

	*result = vnode;
	return 0;
}

int vfs_rename(struct vnode *cwd, const char *oldpath, const char *newpath, uid_t uid)
{
	int error;
	struct vnode *vnode = NULL;
	const char *oldname, *newname;
	struct vnode *oldparent, *newparent;

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
	if (error) {
		vfs_release_vnode(oldparent);
		return error;
	}

	// TODO verify that both locations are on the same device

	error = oldparent->ops->lookup(oldparent, oldname, &vnode);
	if (error) {
		vfs_release_vnode(oldparent);
		vfs_release_vnode(newparent);
		return error;
	}

	error = vnode->ops->rename(vnode, oldparent, oldname, newparent, newname);
	vfs_release_vnode(oldparent);
	vfs_release_vnode(newparent);
	vfs_release_vnode(vnode);
	return error;
}

int vfs_open(struct vnode *cwd, const char *path, int flags, mode_t mode, uid_t uid, struct vfile **file)
{
	int error;
	mode_t required_mode = 0;
	struct vnode *vnode = NULL;

	if (!file)
		return EINVAL;

	error = vfs_lookup(cwd, path, (flags & O_CREAT) ? VLOOKUP_PARENT_OF : VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	if (flags & O_CREAT) {
		const char *filename = path_last_component(path);
		if (!path_valid_component(filename)) {
			vfs_release_vnode(vnode);
			return EINVAL;
		}

		// Lookup the last path component, or create a new file if an error occurs during lookup
		if (vnode->ops->lookup(vnode, filename, &vnode)) {
			// Verify that parent directory is writable
			if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode)) {
				vfs_release_vnode(vnode);
				return EPERM;
			}

			struct vnode *parent = vnode;
			error = parent->ops->create(parent, filename, mode, uid, &vnode);
			vfs_release_vnode(parent);
			if (error) {
				vfs_release_vnode(vnode);
				return error;
			}
		}
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

	if (vnode->mode & S_IFCHR)
		(*file)->ops = &device_vfile_ops;

	error = vnode->ops->fops->open(*file, flags);
	if (error)
		free_fileptr(*file);
	return error;
}

int vfs_close(struct vfile *file)
{
	int error;

	error = file->ops->close(file);
	free_fileptr(file);
	return error;
}

int vfs_read(struct vfile *file, char *buffer, size_t size)
{
	return file->ops->read(file, buffer, size);
}

int vfs_write(struct vfile *file, const char *buffer, size_t size)
{
	return file->ops->write(file, buffer, size);
}

int vfs_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	return file->ops->ioctl(file, request, argp);
}

offset_t vfs_seek(struct vfile *file, offset_t position, int whence)
{
	return file->ops->seek(file, position, whence);
}

int vfs_readdir(struct vfile *file, struct dirent *dir)
{
	return file->ops->readdir(file, dir);
}

struct vfile *vfs_duplicate_fileptr(struct vfile *file)
{
	return dup_fileptr(file);
}

int vfs_release_vnode(struct vnode *vnode)
{
	if (!vnode)
		return 0;
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

