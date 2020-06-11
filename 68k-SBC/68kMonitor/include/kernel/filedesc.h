
#ifndef KERNEL_FILEDESC_H
#define KERNEL_FILEDESC_H

#define OPEN_MAX	4

#include <kernel/inode.h>

struct file {
	struct inode *inode;
};

typedef struct file fd_table_t[OPEN_MAX];

void init_fd_table(fd_table_t table);
int new_fd(fd_table_t table, struct inode *inode);
struct file *get_fd(fd_table_t table, int fd);

#endif
