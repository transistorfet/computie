
#include <stddef.h>

#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>

#include "filedesc.h"


void init_fd_table(fd_table_t table)
{
	for (short i = 0; i < OPEN_MAX; i++)
		table[i] = NULL;
}

void release_fd_table(fd_table_t table)
{
	for (short i = 0; i < OPEN_MAX; i++) {
		if (table[i])
			vfs_close(table[i]);
	}
}

void dup_fd_table(fd_table_t dest, fd_table_t source)
{
	for (short i = 0; i < OPEN_MAX; i++) {
		if (source[i])
			// TODO this needs to be replaced with a new vfs_duplicate() function
			dest[i] = vfs_duplicate_fileptr(source[i]);
	}
}

int find_unused_fd(fd_table_t table)
{
	for (short i = 0; i < OPEN_MAX; i++) {
		if (!table[i])
			return i;
	}
	return EMFILE;
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

void dup_fd(fd_table_t table, int fd, struct vfile *file)
{
	if (fd >= OPEN_MAX)
		return;
	table[fd] = vfs_duplicate_fileptr(file);
}

void unset_fd(fd_table_t table, int fd)
{
	set_fd(table, fd, NULL);
}

