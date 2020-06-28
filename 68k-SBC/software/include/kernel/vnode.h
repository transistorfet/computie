
#ifndef KERNEL_VNODE_H
#define KERNEL_VNODE_H

#include <stdint.h>
#include <sys/types.h>

struct vnode {
	file_mode_t mode;
	uid_t uid;
	gid_t gid;
	offset_t size;
	time_t mtime;
	void *fsdata;

	device_t device;
	inode_t i_num;
	uint8_t refcount;
	char f_dirty;
};

void init_vnode();
struct vnode *new_vnode(device_t dev, file_mode_t mode);
struct vnode *get_vnode(device_t dev, inode_t num);
int free_vnode(struct vnode *vnode);

#endif
