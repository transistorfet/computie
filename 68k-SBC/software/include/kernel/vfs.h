
#ifndef KERNEL_VFS_H
#define KERNEL_VFS_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


struct mount;
struct vnode;
struct vfile;


struct mount_ops {
	int (*mount)(struct mount *mp);				// Mount the filesystem using the pre-allocated struct mount
	int (*umount)(struct mount *mp);			// Unmount the filesystem
	struct vnode *(*root)(struct mount *mp);		// Get the root vnode
	int (*sync)(struct mount *mp);				// Sync data to disk
};


struct vnode_ops {
	struct vfile_ops *fops;

	int (*lookup)(struct vnode *vnode, const char *filename, struct vnode **result);
	int (*mknod)(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, struct vnode **result);
	//mkdir
	//rmdir
	//rename
	//link
	//unlink
	int (*open)(struct vnode *vnode, mode_t mode, struct vfile **file);
};


struct vfile_ops {
	int (*close)(struct vfile *file);
	int (*read)(struct vfile *file, char *buffer, size_t size);
	int (*write)(struct vfile *file, const char *buffer, size_t size);
	int (*ioctl)(struct vfile *file, unsigned int request, void *argp);
	offset_t (*seek)(struct vfile *file, offset_t position, int whence);
	// also readdir?
};


struct mount {
	struct mount_ops *ops;
	//ref to vnode we're mounted on
	//ref to root inode/vnode of this fs
};


struct vnode {
	struct vnode_ops *ops;

	mode_t mode;
	uid_t uid;
	gid_t gid;
	offset_t size;
	time_t mtime;
	void *fsdata;

	device_t device;
	short i_num;
	uint8_t refcount;
	char f_dirty;
};


struct vfile {
	struct vnode *vnode;
	offset_t position;
	int refcount;
};

int vfs_mount(struct mount *mp);
int vfs_umount(struct mount *mp);
struct vnode *vfs_root(struct mount *mp);
int vfs_sync(struct mount *mp);

int vfs_lookup(const char *filename, struct vnode **result);
int vfs_mknod(struct vnode *vnode, const char *filename, mode_t mode, device_t dev, struct vnode **result);
int vfs_open(struct vnode *vnode, mode_t mode, struct vfile **file);

int vfs_close(struct vfile *file);
int vfs_read(struct vfile *file, char *buffer, size_t size);
int vfs_write(struct vfile *file, const char *buffer, size_t size);
int vfs_ioctl(struct vfile *file, unsigned int request, void *argp);
offset_t vfs_seek(struct vfile *file, offset_t position, int whence);

#endif
