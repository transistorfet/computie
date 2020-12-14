
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>

#include <elf.h>

#include "process.h"

extern struct process *current_proc;


int load_binary(const char *path, struct mem_map *map)
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

	error = load_elf_binary(file, map);
	if (error == ENOEXEC) {
		if (!(error = vfs_seek(file, 0, SEEK_SET)))
			return error;
		error = load_flat_binary(file, map);
	}

	vfs_close(file);

	return error;
}

int load_flat_binary(struct vfile *file, struct mem_map *map)
{
	int error;

	// The extra data is for the bss segment, which we don't know the proper size of
	int task_size = file->vnode->size + 0x200;
	char *task_text = kmalloc(task_size);

	if (!(error = vfs_read(file, task_text, task_size)))
		return error;

	// TODO overwriting this could be a memory leak if it's not already NULL.  How do I refcount segments?
	//if (current_proc->map.segments[M_TEXT].base)
	//	kmfree(current_proc->map.segments[M_TEXT].base);
	map->segments[M_TEXT].base = task_text;
	map->segments[M_TEXT].length = task_size;

	return 0;
}


int load_elf_binary(struct vfile *file, struct mem_map *map)
{
	short i;
	int error;
	Elf32_Ehdr header;
	Elf32_Phdr prog_header;

	if (!(error = vfs_read(file, (char *) &header, sizeof(Elf32_Ehdr))))
		return error;

	printk_safe("SIG\n");
	// Look for the ELF signature, 32-bit Big Endian ELF Version 1
	if (memcmp(header.e_ident, "\x7F\x45\x4C\x46\x01\x02\x01", 7)) {
		printk_safe("ekiting\n");
		return ENOEXEC;
	}

	printk_safe("TYPE: %x\n", header.e_type);
	// Make sure it's an executable for the 68k
	if (header.e_type != ET_EXEC || header.e_machine != EM_68K || header.e_phentsize != sizeof(Elf32_Phdr))
		return ENOEXEC;

	printk_safe("PH: %x\n", header.e_phoff);
	if (!(error = vfs_seek(file, header.e_phoff, SEEK_SET)))
		return error;

	for (i = 0; i < header.e_phnum; i++) {
		if (!(error = vfs_read(file, (char *) &prog_header, sizeof(Elf32_Phdr))))
			return error;

		printk_safe("SF: %x\n", prog_header.p_flags);
		if (prog_header.p_flags & PF_X)
			break;
	}
	if (i == header.e_phnum)
		return ENOEXEC;

	printk_safe("MEM: %x %x\n", prog_header.p_offset, prog_header.p_memsz);
	int task_size = prog_header.p_memsz;
	char *task_text = kmalloc(task_size);

	if (!(error = vfs_seek(file, prog_header.p_offset, SEEK_SET)))
		return error;

	if (!(error = vfs_read(file, task_text, prog_header.p_filesz)))
		return error;


	// TODO overwriting this could be a memory leak if it's not already NULL.  How do I refcount segments?
	//if (current_proc->map.segments[M_TEXT].base)
	//	kmfree(current_proc->map.segments[M_TEXT].base);
	map->segments[M_TEXT].base = task_text;
	map->segments[M_TEXT].length = task_size;

	return 0;
}
