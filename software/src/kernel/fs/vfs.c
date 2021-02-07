
#include <string.h>

#include <errno.h>
#include <fcntl.h>
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

void vfs_mount_iter_start(struct mount_iter *iter)
{
	iter->slot = 0;
}

struct mount *vfs_mount_iter_next(struct mount_iter *iter)
{
	struct mount *mp;

	do {
		if (iter->slot >= VFS_MOUNT_MAX)
			return NULL;
		mp = &mountpoints[iter->slot++];
	} while (mp->dev == 0);
	return mp;
}


int vfs_mount(struct vnode *cwd, const char *path, device_t dev, struct mount_ops *ops, int mountflags, uid_t uid)
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

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].dev == dev)
			return EBUSY;
	}

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].dev == 0) {
			mountpoints[i].ops = ops;
			mountpoints[i].mount_node = vnode;
			mountpoints[i].root_node = NULL;
			mountpoints[i].dev = dev;
			mountpoints[i].bits = mountflags;

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
	mp->dev = 0;
	return 0;
}

int vfs_sync(device_t dev)
{
	int error = 0;

	for (short i = 0; i < VFS_MOUNT_MAX; i++) {
		if (mountpoints[i].dev && (!dev || mountpoints[i].dev == dev)) {
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

	cur = (cwd && path[0] != VFS_SEP) ? cwd : root_fs->root_node;
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
			vfs_release_vnode(cur);
			if (!mp)
				return ENXIO;
			cur = vfs_clone_vnode(mp->root_node);
		}

		// Return successfully with the result
		if (path[i] == '\0') {
			*result = cur;
			return 0;
		}

		// This is not the last component, so it must be directory
		if (!S_ISDIR(cur->mode)) {
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

		// If we're supposed to stop before the last component, then skip the lookup and we'll exit from the loop at the top
		if (flags & VLOOKUP_PARENT_OF && path[i] == '\0')
			continue;

		// If we're at the root of a mounted fs and are accessing "..", then swap the root node for the mounted node before lookup
		if (!strcmp(component, "..") && cur == cur->mp->root_node) {
			mp = cur->mp;
			vfs_release_vnode(cur);
			cur = vfs_clone_vnode(mp->mount_node);
		}

		// Call the fs-specific lookup to get the referenced node
		error = cur->ops->lookup(cur, component, &cur);
		if (error) {
			vfs_release_vnode(cur);
			return error;
		}
	}

	return ENOENT;
}

int vfs_reverse_lookup(struct vnode *cwd, char *buf, size_t size, uid_t uid)
{
	int j;
	int len;
	int error;
	struct mount *mp;
	struct vnode *cur;
	struct dirent dir;
	struct vfile *file;

	if (!cwd)
		cwd = root_fs->root_node;

	// TODO this only works for directories, but if we took another vnode arg, we could optionally include its filename
	if (!S_ISDIR(cwd->mode))
		return ENOTDIR;

	j = size;
	buf[--j] = '\0';
	cur = vfs_clone_vnode(cwd);
	while (cur != root_fs->root_node) {
		// If the current node is the fs root node, then switch to the vnode that it's mounted on, or else the inode number wont match
		if (cur == cur->mp->root_node) {
			mp = cur->mp;
			vfs_release_vnode(cur);
			cur = vfs_clone_vnode(mp->mount_node);
		}

		error = vfs_open(cur, "..", O_RDONLY, 0, uid, &file);
		if (error) {
			vfs_release_vnode(cur);
			return error;
		}

		while (1) {
			error = vfs_readdir(file, &dir);
			if (error < 0) {
				vfs_close(file);
				vfs_release_vnode(cur);
				return error;
			}
			else if (error == 0)
				break;

			if (dir.d_ino == cur->ino && strcmp(dir.d_name, ".") && strcmp(dir.d_name, "..")) {
				len = strlen(dir.d_name);
				if (j - len - 1 < 0) {
					vfs_close(file);
					vfs_release_vnode(cur);
					return -1;
				}

				strncpy(&buf[j - len], dir.d_name, len);
				j -= len;
				buf[--j] = VFS_SEP;
				break;
			}
		}

		vfs_release_vnode(cur);
		cur = vfs_clone_vnode(file->vnode);

		vfs_close(file);
	}
	vfs_release_vnode(cur);

	if (buf[j] != '/')
		buf[--j] = VFS_SEP;

	strcpy(buf, &buf[j]);
	return 0;
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

	vnode->mode = (vnode->mode & ~07777) | (mode & 07777);
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

int vfs_link(struct vnode *cwd, const char *oldpath, const char *newpath, uid_t uid)
{
	int error;
	const char *filename;
	struct vnode *vnode = NULL;
	struct vnode *parent = NULL;
	struct vnode *tmpvnode = NULL;

	// TODO We still don't check that there are no loops

	error = vfs_lookup(cwd, oldpath, VLOOKUP_NORMAL, uid, &vnode);
	if (error)
		return error;

	error = vfs_lookup(cwd, newpath, VLOOKUP_PARENT_OF, uid, &parent);
	if (error) {
		vfs_release_vnode(vnode);
		return error;
	}

	filename = path_last_component(newpath);

	// If the target exists, then return an error
	error = parent->ops->lookup(parent, filename, &tmpvnode);
	if (error == ENOENT)
		error = 0;
	else {
		if (!error)
			vfs_release_vnode(tmpvnode);
		error = EEXIST;
	}

	// Verify that parent directory is writable
	if (!error && !verify_mode_access(uid, W_OK, parent->uid, parent->gid, parent->mode))
		error = EACCES;

	// Verify they're on the same mount point
	if (!error && (vnode->mp != parent->mp))
		error = EXDEV;

	if (error) {
		vfs_release_vnode(vnode);
		vfs_release_vnode(parent);
		return error;
	}

	error = parent->ops->link(vnode, parent, filename);
	vfs_release_vnode(parent);
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

	// We look up the file here to check permissions
	filename = path_last_component(path);
	error = parent->ops->lookup(parent, filename, &vnode);
	if (error) {
		vfs_release_vnode(parent);
		return error;
	}

	// Verify that the file we're trying to delete is writable
	if (!verify_mode_access(uid, W_OK, vnode->uid, vnode->gid, vnode->mode))
		error = EPERM;

	if (!error)
		// unlink does not take ownership of vnode and must not call vfs_release_vnode
		error = parent->ops->unlink(parent, vnode, filename);

	vfs_release_vnode(parent);
	vfs_release_vnode(vnode);
	return error;
}

static inline int _rename_find_parent(struct vnode *cwd, const char *path, uid_t uid, struct vnode **result)
{
	int error;
	struct vnode *vnode;

	// Look up the parent (if it's not a directory, we'll return with an error)
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

	if (!(mode & S_IFMT))
		mode |= S_IFREG;

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

	if (flags & O_TRUNC && S_ISREG(vnode->mode))
		vnode->ops->truncate(vnode);

	if (flags & O_APPEND && S_ISREG(vnode->mode))
		(*file)->position = (*file)->vnode->size;

	if (S_ISDEV(vnode->mode))
		(*file)->ops = &device_vfile_ops;

	error = (*file)->ops->open(*file, flags);
	if (error)
		free_fileptr(*file);
	return error;
}

int vfs_close(struct vfile *file)
{
	int error = 0;

	if (file->refcount <= 1)
		error = file->ops->close(file);
	free_fileptr(file);
	return error;
}

int vfs_read(struct vfile *file, char *buffer, size_t size)
{
	if ((file->flags & O_ACCMODE) == O_WRONLY)
		return EACCES;
	return file->ops->read(file, buffer, size);
}

int vfs_write(struct vfile *file, const char *buffer, size_t size)
{
	if ((file->flags & O_ACCMODE) == O_RDONLY)
		return EACCES;
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
	if (path[i] == VFS_SEP)
		i--;
	for (; i >= 0; i--) {
		if (path[i] == VFS_SEP)
			break;
	}
	i += 1;
	return &path[i];
}

int path_valid_component(const char *path)
{
	if (*path == '\0')
		return 0;
	for (; *path; path++) {
		if (*path <= 0x20 || *path == '/')
			return 0;
	}
	return 1;
}

