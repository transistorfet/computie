
#include <stddef.h>

#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>

#include "filedesc.h"


void init_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++)
		table[i] = NULL;
}

void release_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		if (table[i])
			free_fileptr(table[i]);
	}
}

void dup_fd_table(fd_table_t dest, fd_table_t source)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		if (source[i])
			dest[i] = dup_fileptr(source[i]);
	}
}



int find_unused_fd(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		if (!table[i])
			return i;
	}
	return EMFILE;
}

void free_fd(fd_table_t table, int fd)
{
	free_fileptr(table[fd]);
	table[fd] = NULL;
}

struct vfile *get_fd(fd_table_t table, int fd)
{
	if (fd >= OPEN_MAX || !table[fd]->vnode)
		return NULL;
	return table[fd];
}

void set_fd(fd_table_t table, int fd, struct vfile *file)
{
	if (fd >= OPEN_MAX)
		return;
	table[fd] = file;
}

