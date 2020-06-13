
#ifndef KERNEL_INODE_H
#define KERNEL_INODE_H

#include <stdint.h>
#include <sys/types.h>

struct inode {
	file_mode_t mode;

	device_t device;
	inode_t i_num;
	uint8_t refcount;
};

void init_inode();
struct inode *new_inode(device_t dev, file_mode_t mode);
struct inode *get_inode(device_t dev, inode_t num);
int free_inode(struct inode *inode);

#endif
