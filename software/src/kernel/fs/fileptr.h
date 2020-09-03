
#ifndef _SRC_KERNEL_FS_FILEPTR_H
#define _SRC_KERNEL_FS_FILEPTR_H

void init_fileptr_table();
struct vfile *new_fileptr(struct vnode *vnode, int flags);
struct vfile *dup_fileptr(struct vfile *file);
void free_fileptr(struct vfile *file);

#endif

