
#include <stddef.h>
#include <kernel/vfs.h>

#include "../printk.h"

#define MAX_VNODES	20

static struct vnode vnode_table[MAX_VNODES];

void init_vnode()
{
	for (char i = 0; i < MAX_VNODES; i++) {
		vnode_table[i].refcount = 0;
	}
}

// TODO this function should take a path, but I have no path mapping yet, so take a device_t
//struct inode *new_inode(char *path, mode_t mode)
struct vnode *new_vnode(device_t dev, mode_t mode, struct vnode_ops *ops)
{
	for (char i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].refcount <= 0) {
			vnode_table[i].ops = ops;
			vnode_table[i].refcount = 1;
			vnode_table[i].mode = mode;
			vnode_table[i].size = 0;
			vnode_table[i].device = dev;
			vnode_table[i].block = NULL;
			return &vnode_table[i];
		}
	}
	return NULL;
}

struct vnode *get_vnode(device_t dev)
{
	for (char i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].device == dev) {
			// TODO is this supposed to increment the refcount
			return &vnode_table[i];
		}
	}
	return NULL;
}

void free_vnode(struct vnode *vnode)
{
	vnode->refcount--;
	if (vnode->refcount < 0)
		printk("Error: double free of vnode, %x\n", vnode);
}



