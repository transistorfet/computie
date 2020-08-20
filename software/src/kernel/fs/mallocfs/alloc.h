
#ifndef _SRC_KERNEL_FS_MALLOCFS_ALLOC_H
#define _SRC_KERNEL_FS_MALLOCFS_ALLOC_H

#include <stdlib.h>

#include "mallocfs.h"

#define MFS_ALLOC_BLOCK()	((struct mallocfs_block *) malloc(MALLOCFS_BLOCK_SIZE))
#define MFS_FREE_BLOCK(ptr)	free(ptr)

#define MFS_ZERO	1

void init_mallocfs_alloc();
struct vnode *new_mallocfs_vnode(device_t dev, mode_t mode);
struct mallocfs_block *new_mallocfs_block(char zero);
struct vnode *new_mallocfs_vnode_with_block(mode_t mode, struct vnode *parent);

#endif
