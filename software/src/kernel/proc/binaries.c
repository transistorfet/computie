
#include <errno.h>
#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/kmalloc.h>

#include "process.h"

extern struct process *current_proc;

int load_flat_binary(const char *path, struct mem_map *map)
{
	int error;
	struct vfile *file;

	if (vfs_access(current_proc->cwd, path, X_OK, current_proc->uid))
		return EPERM;

	if ((error = vfs_open(current_proc->cwd, path, O_RDONLY, 0, current_proc->uid, &file)) < 0) {
		printk("Error opening %s: %d\n", path, error);
		return error;
	}

	if ((file->vnode->mode & (S_IFDIR | S_IFCHR | S_IFIFO)) != 0) {
		vfs_close(file);
		return EISDIR;
	}

	// The extra data is for the bss segment, which we don't know the proper size of
	int task_size = file->vnode->size + 0x200;
	char *task_text = kmalloc(task_size);

	error = vfs_read(file, task_text, task_size);
	vfs_close(file);

	if (error < 0) {
		printk("Error reading file %s: %d\n", path, error);
		return error;
	}


	// TODO overwriting this could be a memory leak if it's not already NULL.  How do I refcount segments?
	//if (current_proc->map.segments[M_TEXT].base)
	//	kmfree(current_proc->map.segments[M_TEXT].base);
	map->segments[M_TEXT].base = task_text;
	map->segments[M_TEXT].length = task_size;

	return 0;
}

