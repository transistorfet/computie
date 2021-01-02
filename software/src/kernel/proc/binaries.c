
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>

#include <elf.h>

#include "process.h"

extern struct process *current_proc;

int load_flat_binary(struct vfile *file, struct process *proc, void **entry);
int load_elf_binary(struct vfile *file, struct process *proc, void **entry);

int load_binary(const char *path, struct process *proc, void **entry)
{
	int error;
	struct vfile *file;

	if (vfs_access(proc->cwd, path, X_OK, proc->uid))
		return EPERM;

	if ((error = vfs_open(proc->cwd, path, O_RDONLY, 0, proc->uid, &file)) < 0)
		return error;

	if ((file->vnode->mode & (S_IFDIR | S_IFCHR | S_IFIFO)) != 0) {
		vfs_close(file);
		return EISDIR;
	}

	error = load_elf_binary(file, proc, entry);
	// If the file was not a valid ELF binary, then execute it as a flat binary
	if (error == ENOEXEC) {
		error = vfs_seek(file, 0, SEEK_SET);
		if (!error) {
			error = load_flat_binary(file, proc, entry);
		}
	}

	vfs_close(file);

	return error;
}

int load_flat_binary(struct vfile *file, struct process *proc, void **entry)
{
	int error;

	// The extra data is for the bss segment, which we don't know the proper size of
	int task_size = file->vnode->size + 0x200;
	create_process_memory(proc, task_size);

	if (!(error = vfs_read(file, proc->map.segments[M_TEXT].base, task_size)))
		return error;

	*entry = proc->map.segments[M_TEXT].base;

	return 0;
}


int load_elf_binary(struct vfile *file, struct process *proc, void **entry)
{
	short i;
	int error;
	Elf32_Ehdr header;
	Elf32_Phdr prog_header;

	if (!(error = vfs_read(file, (char *) &header, sizeof(Elf32_Ehdr))))
		return error;

	// Look for the ELF signature, 32-bit Big Endian ELF Version 1
	if (memcmp(header.e_ident, "\x7F\x45\x4C\x46\x01\x02\x01", 7))
		return ENOEXEC;

	// Make sure it's an executable for the 68k
	if (header.e_type != ET_EXEC || header.e_machine != EM_68K || header.e_phentsize != sizeof(Elf32_Phdr))
		return ENOEXEC;

	if (!(error = vfs_seek(file, header.e_phoff, SEEK_SET)))
		return error;

	for (i = 0; i < header.e_phnum; i++) {
		if (!(error = vfs_read(file, (char *) &prog_header, sizeof(Elf32_Phdr))))
			return error;

		if (prog_header.p_flags & PF_X)
			break;
	}
	if (i == header.e_phnum)
		return ENOEXEC;

	create_process_memory(proc, prog_header.p_memsz);

	if (!(error = vfs_seek(file, prog_header.p_offset, SEEK_SET)))
		return error;

	if (!(error = vfs_read(file, proc->map.segments[M_TEXT].base, prog_header.p_filesz)))
		return error;

	*entry = proc->map.segments[M_TEXT].base + header.e_entry;

	return 0;
}
