
#include <stddef.h>
#include <kernel/inode.h>

#define MAX_INODES	10

static struct inode inode_table[MAX_INODES];

void init_inode()
{
	for (char i = 0; i < MAX_INODES; i++) {
		inode_table[i].refcount = 0;
	}
}

//struct inode *new_inode(char *path, file_mode_t mode)
struct inode *new_inode(device_t dev, file_mode_t mode)
{
	for (char i = 0; i < MAX_INODES; i++) {
		if (inode_table[i].refcount == 0) {
			inode_table[i].mode = mode;
			inode_table[i].device = dev;
			inode_table[i].i_num = 0;
			inode_table[i].refcount = 1;
			return &inode_table[i];
		}
	}
	return NULL;
}

struct inode *get_inode(device_t dev, inode_t num)
{

}

int free_inode(struct inode *inode)
{

}



