
#ifndef _SRC_KERNEL_FILEDESC_H
#define _SRC_KERNEL_FILEDESC_H

#define OPEN_MAX	10

#include <kernel/vfs.h>

typedef struct vfile *fd_table_t[OPEN_MAX];

void init_fd_table(fd_table_t table);
void release_fd_table(fd_table_t table);
void dup_fd_table(fd_table_t dest, fd_table_t source);
int find_unused_fd(fd_table_t table);
struct vfile *get_fd(fd_table_t table, int fd);
void set_fd(fd_table_t table, int fd, struct vfile *file);
void unset_fd(fd_table_t table, int fd);

#endif
