
#ifndef KERNEL_VFS_H
#define KERNEL_VFS_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


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


struct mount;
struct vnode;
struct vfile;
struct vdir;


struct mount_ops {
	int (*mount)(struct mount *mp);				// Mount the filesystem using the pre-allocated struct mount
	int (*umount)(struct mount *mp);			// Unmount the filesystem
	int (*root)(struct mount *mp, struct vnode **result);	// Get the root vnode
	int (*sync)(struct mount *mp);				// Sync data to disk
};


struct vnode_ops {
	struct vfile_ops *fops;

	int (*create)(struct vnode *vnode, const char *filename, mode_t mode, struct vnode **result);
	int (*mknod)(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, struct vnode **result);
	int (*lookup)(struct vnode *vnode, const char *filename, struct vnode **result);
	int (*unlink)(struct vnode *parent, struct vnode *vnode);
	//rename
	//mkdir (or can it be done through create)
	//rmdir (or can it be done through unlink)
	//link
	int (*truncate)(struct vnode *vnode);			// Truncate the file data (size should be 0 after)
	int (*release)(struct vnode *vnode);			// Release the vnode
};

// TODO should you just integrate this with the vnode ops
struct vfile_ops {
	int (*open)(struct vfile *file, int flags);
	int (*close)(struct vfile *file);
	int (*read)(struct vfile *file, char *buffer, size_t size);
	int (*write)(struct vfile *file, const char *buffer, size_t size);
	int (*ioctl)(struct vfile *file, unsigned int request, void *argp);
	offset_t (*seek)(struct vfile *file, offset_t position, int whence);
	int (*readdir)(struct vfile *file, struct vdir *dir);
};


struct mount {
	struct mount_ops *ops;
	struct vnode *mount_node;	// The vnode this fs is mounted on
	struct vnode *root_node;	// the root vnode of this fs
};

struct vnode {
	struct vnode_ops *ops;
	short refcount;

	mode_t mode;
	uid_t uid;
	gid_t gid;
	offset_t size;
	time_t mtime;

	struct mount *mounted;		// TODO what do you think about this? not sure

	union {} data;
};

struct vfile {
	struct vnode *vnode;
	int refcount;
	int flags;
	offset_t position;
};

struct vdir {
	struct vnode *vnode;
	char name[VFS_FILENAME_MAX];
};

#define VLOOKUP_NORMAL		0000
#define VLOOKUP_PARENT_OF	0001

int init_vfs();

int vfs_mount(struct mount *mp);
int vfs_umount(struct mount *mp);
int vfs_sync(struct mount *mp);

int vfs_mknod(const char *path, mode_t mode, device_t dev, struct vnode **result);
int vfs_lookup(const char *path, int flags, struct vnode **result);
int vfs_unlink(const char *path);
int vfs_open(const char *path, int flags, mode_t mode, struct vfile **file);

int vfs_close(struct vfile *file);
int vfs_read(struct vfile *file, char *buffer, size_t size);
int vfs_write(struct vfile *file, const char *buffer, size_t size);
int vfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t vfs_seek(struct vfile *file, offset_t position, int whence);
int vfs_readdir(struct vfile *file, struct vdir *dir);

int vfs_release_vnode(struct vnode *vnode);

const char *path_last_component(const char *path);


static inline void vfs_init_vnode(struct vnode *vnode, struct vnode_ops *ops, mode_t mode, uid_t uid, gid_t gid, offset_t size, time_t mtime)
{
	vnode->ops = ops;
	vnode->refcount = 1;
	vnode->mode = mode;
	vnode->uid = uid;
	vnode->gid = gid;
	vnode->size = size;
	vnode->mtime = mtime;
}

#endif

