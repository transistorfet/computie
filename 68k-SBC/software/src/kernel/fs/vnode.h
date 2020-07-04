
#ifndef _SRC_KERNEL_VNODE_H
#define _SRC_KERNEL_VNODE_H

#include <kernel/vfs.h>

void init_vnode();
struct vnode *new_vnode(device_t dev, mode_t mode, struct vnode_ops *ops);
struct vnode *get_vnode(device_t dev, short num);
int free_vnode(struct vnode *vnode);

#endif
