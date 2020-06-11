
#include <kernel/filedesc.h>


void init_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		table[i].inode = 0;
	}
}

int new_fd(fd_table_t table, struct inode *inode)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		if (!table[i].inode) {
			table[i].inode = inode;
			return i;
		}
	}
}

struct file *get_fd(fd_table_t table, int fd)
{
	return &table[fd];
}

