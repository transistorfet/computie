
#include <stddef.h>

#include <errno.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>

#include "fileptr.h"

#define FILE_TABLE_MAX		16


struct vfile file_table[FILE_TABLE_MAX];


void init_fileptr_table()
{
	for (char i = 0; i < FILE_TABLE_MAX; i++) {
		file_table[i].ops = NULL;
		file_table[i].vnode = NULL;
		file_table[i].refcount = 0;
	}
}

struct vfile *new_fileptr(struct vnode *vnode, int flags)
{
	for (char i = 0; i < FILE_TABLE_MAX; i++) {
		if (!file_table[i].vnode) {
			file_table[i].ops = vnode->ops->fops;
			file_table[i].vnode = vnode;
			file_table[i].position = 0;
			file_table[i].refcount = 1;
			file_table[i].flags = flags;
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
	--file->refcount;
	if (file->refcount < 0) {
		printk("Error: double free of file pointer, %x\n", file);
	}
	else if (file->refcount <= 0) {
		vfs_release_vnode(file->vnode);
		file->vnode = NULL;
	}
}


