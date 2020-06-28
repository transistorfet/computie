
#include <stddef.h>
#include <kernel/vnode.h>

#define MAX_INODES	10

static struct vnode vnode_table[MAX_INODES];

void init_vnode()
{
	for (char i = 0; i < MAX_INODES; i++) {
		vnode_table[i].refcount = 0;
	}
}

// TODO this function should take a path, but I have no path mapping yet, so take a device_t
//struct inode *new_inode(char *path, file_mode_t mode)
struct vnode *new_vnode(device_t dev, file_mode_t mode)
{
	for (char i = 0; i < MAX_INODES; i++) {
		if (vnode_table[i].refcount == 0) {
			vnode_table[i].mode = mode;
			vnode_table[i].device = dev;
			vnode_table[i].i_num = 0;
			vnode_table[i].refcount = 1;
			return &vnode_table[i];
		}
	}
	return NULL;
}

struct vnode *get_vnode(device_t dev, inode_t num)
{
	for (char i = 0; i < MAX_INODES; i++) {
		if (vnode_table[i].device == dev && vnode_table[i].i_num == num) {
			// TODO is this supposed to increment the refcount
			return &vnode_table[i];
		}
	}
	return NULL;
}

int free_vnode(struct vnode *vnode)
{
	vnode->refcount--;
	return 0;
}



