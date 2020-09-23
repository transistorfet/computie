
#ifndef _SRC_KERNEL_FS_MALLOCFS_ALLOC_H
#define _SRC_KERNEL_FS_MALLOCFS_ALLOC_H

#include <kernel/kmalloc.h>

#include "mallocfs.h"

#define MFS_ALLOC_BLOCK()	((struct mallocfs_block *) kmalloc(MALLOCFS_BLOCK_SIZE))
#define MFS_FREE_BLOCK(ptr)	kmfree(ptr)

#define MFS_ZERO	1

void init_mallocfs_alloc();
struct vnode *new_mallocfs_vnode(struct mount *mp, device_t dev, mode_t mode, uid_t uid);
struct mallocfs_block *new_mallocfs_block(char zero);

#endif
