
#ifndef KERNEL_FILEDESC_H
#define KERNEL_FILEDESC_H

#define OPEN_MAX	4

#include <kernel/vnode.h>

struct file {
	struct vnode *vnode;
	offset_t position;
};

typedef struct file fd_table_t[OPEN_MAX];

void init_fd_table(fd_table_t table);
int new_fd(fd_table_t table, struct vnode *vnode);
void free_fd(fd_table_t table, int fd);
struct file *get_fd(fd_table_t table, int fd);

#endif
