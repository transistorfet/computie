
#ifndef KERNEL_VFS_H
#define KERNEL_VFS_H

#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <kernel/time.h>

#ifndef F_OK
#define F_OK		0	// Test if file exists
#endif
#ifndef X_OK
#define X_OK		1	// Test if file is executable
#endif
#ifndef W_OK
#define W_OK		2	// Test if file is writable
#endif
#ifndef R_OK
#define R_OK		4	// Test if file is readable
#endif

#ifndef SEEK_SET
#define SEEK_SET	0	// Seek relative to the beginning of file
#endif
#ifndef SEEK_CUR
#define SEEK_CUR	1	// Seek relative to the current position
#endif
#ifndef SEEK_END
#define SEEK_END	2	// Seek relative to the end of file
#endif

#define VFS_SEP			'/'
#define VFS_FILENAME_MAX	14

// Mount bitflags
#define VFS_MBF_READ_ONLY	0x0001

// Vnode bitflags
#define VBF_MOUNTED		0x0001
#define VBF_DIRTY		0x0002

#define VFS_POLL_READ		0x01
#define VFS_POLL_WRITE		0x02
#define VFS_POLL_ERROR		0x04
#define VFS_POLL_HUP		0x08

#define ATIME			0x01
#define MTIME			0x02
#define CTIME			0x04


struct mount;
struct vnode;
struct vfile;


struct mount_ops {
	char *fstype;								// Filesystem Type Name (used by mount syscall)
	int (*init)();								// Initialize the filesystem at boot
	int (*mount)(struct mount *mp, device_t dev, struct vnode *parent);	// Mount the filesystem using the pre-allocated struct mount
	int (*unmount)(struct mount *mp);					// Unmount the filesystem
	int (*sync)(struct mount *mp);						// Sync data to disk
};


struct vnode_ops {
	struct vfile_ops *fops;

	int (*create)(struct vnode *vnode, const char *filename, mode_t mode, uid_t uid, struct vnode **result);
	int (*mknod)(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, uid_t uid, struct vnode **result);
	// NOTE lookup must call vfs_release_vnode on the existing *result vnode if it is non-NULL.  This makes it easier to swap vnode references when resolving paths
	int (*lookup)(struct vnode *vnode, const char *filename, struct vnode **result);
	int (*link)(struct vnode *oldvnode, struct vnode *newparent, const char *filename);
	int (*unlink)(struct vnode *parent, struct vnode *vnode, const char *filename);
	int (*rename)(struct vnode *vnode, struct vnode *oldparent, const char *oldname, struct vnode *newparent, const char *newname);
	int (*truncate)(struct vnode *vnode);			// Truncate the file data (size should be 0 after)
	int (*update)(struct vnode *vnode);
	int (*release)(struct vnode *vnode);			// Release the vnode
};

struct vfile_ops {
	int (*open)(struct vfile *file, int flags);
	int (*close)(struct vfile *file);
	int (*read)(struct vfile *file, char *buffer, size_t size);
	int (*write)(struct vfile *file, const char *buffer, size_t size);
	int (*ioctl)(struct vfile *file, unsigned int request, void *argp, uid_t uid);
	int (*poll)(struct vfile *file, int events);
	offset_t (*seek)(struct vfile *file, offset_t position, int whence);
	int (*readdir)(struct vfile *file, struct dirent *dir);
};

struct mount_iter {
	short slot;
};

struct mount {
	struct mount_ops *ops;
	struct vnode *mount_node;	// The vnode this fs is mounted on
	struct vnode *root_node;	// The root vnode of this fs
	void *super;			// The fs-specific superblock data
	device_t dev;			// The device that's mounted
	uint16_t bits;			// Bitflags for this mountpoint
};

struct vnode {
	struct vnode_ops *ops;
	struct mount *mp;		// The mountpoint this vnode belongs to
	int refcount;			// Number of references to this vnode currently in use

	mode_t mode;
	short nlinks;
	uid_t uid;
	gid_t gid;
	uint16_t bits;
	device_t rdev;
	inode_t ino;
	offset_t size;

	time_t atime;
	time_t mtime;
	time_t ctime;

	union {} data;			// Filesystem-specific data follows the vnode
};

struct vfile {
	struct vfile_ops *ops;
	struct vnode *vnode;
	short refcount;
	short flags;
	offset_t position;
};

#define VLOOKUP_NORMAL		0000
#define VLOOKUP_PARENT_OF	0001

int init_vfs();

void vfs_mount_iter_start(struct mount_iter *iter);
struct mount *vfs_mount_iter_next(struct mount_iter *iter);
int vfs_mount(struct vnode *cwd, const char *path, device_t dev, struct mount_ops *ops, int mountflags, uid_t uid);
int vfs_unmount(device_t dev, uid_t uid);
int vfs_sync(device_t dev);

int vfs_lookup(struct vnode *cwd, const char *path, int flags, uid_t uid, struct vnode **result);
int vfs_reverse_lookup(struct vnode *cwd, char *buf, size_t size, uid_t uid);
int vfs_access(struct vnode *cwd, const char *path, int mode, uid_t uid);
int vfs_chmod(struct vnode *cwd, const char *path, int mode, uid_t uid);
int vfs_chown(struct vnode *cwd, const char *path, uid_t owner, gid_t group, uid_t uid);
int vfs_mknod(struct vnode *cwd, const char *path, mode_t mode, device_t dev, uid_t uid, struct vnode **result);
int vfs_link(struct vnode *cwd, const char *oldpath, const char *newpath, uid_t uid);
int vfs_unlink(struct vnode *cwd, const char *path, uid_t uid);
int vfs_rename(struct vnode *cwd, const char *oldpath, const char *newpath, uid_t uid);
int vfs_open(struct vnode *cwd, const char *path, int flags, mode_t mode, uid_t uid, struct vfile **file);

int vfs_close(struct vfile *file);
int vfs_read(struct vfile *file, char *buffer, size_t size);
int vfs_write(struct vfile *file, const char *buffer, size_t size);
int vfs_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid);
int vfs_poll(struct vfile *file, int events);
offset_t vfs_seek(struct vfile *file, offset_t position, int whence);
int vfs_readdir(struct vfile *file, struct dirent *dir);
struct vfile *vfs_duplicate_fileptr(struct vfile *file);

int vfs_create_pipe(struct vfile **rfile, struct vfile **wfile);

const char *path_last_component(const char *path);
int path_valid_component(const char *path);


static inline void vfs_init_vnode(
	struct vnode *vnode,
	struct vnode_ops *ops,
	struct mount *mp,
	mode_t mode,
	short nlinks,
	uid_t uid,
	gid_t gid,
	device_t rdev,
	inode_t ino,
	offset_t size,
	time_t atime,
	time_t mtime,
	time_t ctime)
{
	vnode->ops = ops;
	vnode->mp = mp;
	vnode->refcount = 1;
	vnode->mode = mode;
	vnode->nlinks = nlinks;
	vnode->uid = uid;
	vnode->gid = gid;
	vnode->bits = 0;
	vnode->rdev = rdev;
	vnode->ino = ino;
	vnode->size = size;

	vnode->atime = atime;
	vnode->mtime = mtime;
	vnode->ctime = ctime;
}

static inline struct vnode *vfs_clone_vnode(struct vnode *vnode)
{
	vnode->refcount++;
	return vnode;
}

int vfs_release_vnode(struct vnode *vnode);

static inline void vfs_update_time(struct vnode *vnode, char update)
{
	// Only update timestamps if the filesystem is not mounted read-only
	if (vnode->mp->bits & VFS_MBF_READ_ONLY)
		return;

	time_t t = get_system_time();
	if (update & ATIME)
		vnode->atime = t;
	if (update & MTIME)
		vnode->mtime = t;
	if (update & CTIME)
		vnode->ctime = t;
	vnode->bits |= VBF_DIRTY;
}

#endif

