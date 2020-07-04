
#include <stddef.h>

#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/filedesc.h>


#define FILE_TABLE_MAX		16


struct vfile file_table[FILE_TABLE_MAX];


void init_file_table()
{
	for (char i = 0; i < FILE_TABLE_MAX; i++) {
		file_table[i].vnode = NULL;
	}
}

struct vfile *new_file(struct vnode *vnode)
{
	for (char i = 0; i < FILE_TABLE_MAX; i++) {
		if (!file_table[i].vnode) {
			file_table[i].vnode = vnode;
			file_table[i].position = 0;
			file_table[i].refcount = 1;
			return &file_table[i];
		}
	}
	return NULL;
}

void free_file(struct vfile *file)
{
	file->vnode = NULL;
}



void init_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++)
		table[i] = NULL;
}

void dup_fd_table(fd_table_t dest, fd_table_t source)
{
	for (char i = 0; i < OPEN_MAX; i++)
		dest[i] = source[i];
}

int find_unused_fd(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++) {
		if (!table[i])
			return i;
	}
	return -EMFILE;
}

void free_fd(fd_table_t table, int fd)
{
	table[fd]->refcount -= 1;
	if (table[fd]->refcount <= 0)
		free_file(table[fd]);
	table[fd] = NULL;
}

struct vfile *get_fd(fd_table_t table, int fd)
{
	if (fd >= FILE_TABLE_MAX || !table[fd]->vnode)
		return NULL;
	return table[fd];
}

void set_fd(fd_table_t table, int fd, struct vfile *file)
{
	if (fd >= FILE_TABLE_MAX)
		return;
	table[fd] = file;
}

