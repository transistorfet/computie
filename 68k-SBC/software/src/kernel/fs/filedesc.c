
#include <stddef.h>

#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/filedesc.h>


#define FILE_TABLE_MAX		16


struct vfile file_table[FILE_TABLE_MAX];


void init_fileptr_table()
{
	for (char i = 0; i < FILE_TABLE_MAX; i++) {
		file_table[i].vnode = NULL;
	}
}

struct vfile *new_fileptr(struct vnode *vnode)
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

struct vfile *dup_fileptr(struct vfile *file)
{
	file->refcount++;
	return file;
}

void free_fileptr(struct vfile *file)
{
	if (--file->refcount <= 0)
		file->vnode = NULL;
}




void init_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++)
		table[i] = NULL;
}

void release_fd_table(fd_table_t table)
{
	for (char i = 0; i < OPEN_MAX; i++)
		free_fileptr(table[i]);
}

void dup_fd_table(fd_table_t dest, fd_table_t source)
{
	for (char i = 0; i < OPEN_MAX; i++)
		dest[i] = dup_fileptr(source[i]);
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
	table[fd]->refcount -= 1;
	if (table[fd]->refcount <= 0)
		free_fileptr(table[fd]);
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

