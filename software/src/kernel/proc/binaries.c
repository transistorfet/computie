
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/printk.h>

#include <elf.h>

#include "memory.h"
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

	if (!S_ISREG(file->vnode->mode)) {
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

	if ((error = vfs_read(file, proc->map.segments[M_TEXT].base, task_size)) <= 0)
		return error;

	*entry = proc->map.segments[M_TEXT].base;

	return 0;
}

#define PROG_HEADER_MAX	    6

int load_elf_binary(struct vfile *file, struct process *proc, void **entry)
{
	int error;
	short num_ph;
	size_t mem_size;
	Elf32_Ehdr header;
	Elf32_Phdr prog_headers[PROG_HEADER_MAX];

	if (!(error = vfs_read(file, (char *) &header, sizeof(Elf32_Ehdr))))
		return error;

	// Look for the ELF signature, 32-bit Big Endian ELF Version 1
	if (memcmp(header.e_ident, "\x7F\x45\x4C\x46\x01\x02\x01", 7))
		return ENOEXEC;

	// Make sure it's an executable for the 68k
	if (header.e_type != ET_EXEC || header.e_machine != EM_68K || header.e_phentsize != sizeof(Elf32_Phdr))
		return ENOEXEC;

	// Load the program headers from the ELF file
	num_ph = header.e_phnum <= PROG_HEADER_MAX ? header.e_phnum : PROG_HEADER_MAX;
	if (!(error = vfs_seek(file, header.e_phoff, SEEK_SET)))
		return error;
	if (!(error = vfs_read(file, (char *) prog_headers, sizeof(Elf32_Phdr) * num_ph)))
		return error;

	// Calculate the total size of memory to allocate (not including the stack)
	mem_size = 0;
	for (short i = 0; i < num_ph; i++) {
		if (prog_headers[i].p_type == PT_LOAD) {
			if (prog_headers[i].p_vaddr > mem_size)
				mem_size = prog_headers[i].p_vaddr;
			mem_size += prog_headers[i].p_memsz;
		}
	}
	if (!mem_size)
		return ENOEXEC;

	// Allocate the process memory and initialize the memory maps
	create_process_memory(proc, mem_size);

	// Load all the program segments
	for (short i = 0; i < num_ph; i++) {
		if (prog_headers[i].p_type == PT_LOAD && prog_headers[i].p_filesz > 0) {
			if ((error = vfs_seek(file, prog_headers[i].p_offset, SEEK_SET)) < 0)
				return EKILL;
			if ((error = vfs_read(file, proc->map.segments[M_TEXT].base + prog_headers[i].p_vaddr, prog_headers[i].p_filesz)) < 0)
				return EKILL;
		}
		else if (prog_headers[i].p_type == PT_GNU_RELRO) {
			char **data = proc->map.segments[M_TEXT].base + prog_headers[i].p_vaddr;
			for (int entries = prog_headers[i].p_memsz >> 2; entries; entries--, data++)
				*data = (char *) proc->map.segments[M_TEXT].base + (size_t) *data;
		}
	}

	*entry = proc->map.segments[M_TEXT].base + header.e_entry;

	return 0;
}
