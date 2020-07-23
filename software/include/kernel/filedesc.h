
#ifndef KERNEL_FILEDESC_H
#define KERNEL_FILEDESC_H

#define OPEN_MAX	4

#include <kernel/vfs.h>

typedef struct vfile *fd_table_t[OPEN_MAX];

void init_fileptr_table();
struct vfile *new_fileptr(struct vnode *vnode);
struct vfile *dup_fileptr(struct vfile *file);
void free_fileptr(struct vfile *file);

void init_fd_table(fd_table_t table);
void release_fd_table(fd_table_t table);
void dup_fd_table(fd_table_t dest, fd_table_t source);
int find_unused_fd(fd_table_t table);
void free_fd(fd_table_t table, int fd);
struct vfile *get_fd(fd_table_t table, int fd);
void set_fd(fd_table_t table, int fd, struct vfile *file);

#endif
