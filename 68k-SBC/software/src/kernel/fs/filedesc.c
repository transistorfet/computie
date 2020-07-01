
#include <stddef.h>

#include <errno.h>
#include <kernel/filedesc.h>


void init_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		table[i].vnode = 0;
	}
}

int new_fd(fd_table_t table, struct vnode *vnode)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		if (!table[i].vnode) {
			table[i].vnode = vnode;
			table[i].position = 0;
			return i;
		}
	}
}

void free_fd(fd_table_t table, int fd)
{
	struct file *f = get_fd(table, fd);
	if (f)
		f->vnode = NULL;
}

struct file *get_fd(fd_table_t table, int fd)
{
	if (fd >= OPEN_MAX || !table[fd].vnode)
		return NULL;
	return &table[fd];
}

