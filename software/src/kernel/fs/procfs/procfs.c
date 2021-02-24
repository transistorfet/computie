
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <kernel/vfs.h>
#include <kernel/driver.h>

#include "../../proc/process.h"

#include "data.h"
#include "procfs.h"
#include "../nop.h"

#define PROCFS_POSITION(x)	((struct procfs_position *) &(x))

struct vfile_ops procfs_vfile_ops = {
	procfs_open,
	procfs_close,
	procfs_read,
	procfs_write,
	procfs_ioctl,
	nop_poll,
	procfs_seek,
	procfs_readdir,
};

struct vnode_ops procfs_vnode_ops = {
	&procfs_vfile_ops,
	nop_create,
	nop_mknod,
	procfs_lookup,
	nop_link,
	nop_unlink,
	nop_rename,
	nop_truncate,
	nop_update,
	procfs_release,
};

struct mount_ops procfs_mount_ops = {
	"procfs",
	procfs_init,
	procfs_mount,
	procfs_unmount,
	nop_sync,
};

struct procfs_dir_entry root_files[] = {
	{ PFN_ROOTDIR,	".",		NULL },
	{ PFN_ROOTDIR,	"..",		NULL },
	{ PFN_MOUNTS,	"mounts",	get_data_mounts },
	{ 0, NULL, NULL },
};

struct procfs_dir_entry proc_files[] = {
	{ PFN_PROCDIR,	".",		NULL },
	{ PFN_ROOTDIR,	"..",		NULL },
	{ PFN_CMDLINE,	"cmdline",	get_data_cmdline },
	{ PFN_STAT,	"stat", 	get_data_stat },
	{ PFN_STATM,	"statm",	get_data_statm },
	{ 0, NULL, NULL },
};

struct procfs_position {
	uint16_t slot;
	struct process_iter iter;
};


#define MAX_VNODES	6
#define MAX_BUFFER	256

static struct procfs_vnode vnode_table[MAX_VNODES];

static procfs_filenum_t _find_filenum_by_name(struct procfs_dir_entry *entries, const char *filename);
static struct procfs_dir_entry *_get_entry_by_num(struct procfs_dir_entry *entries, procfs_filenum_t filenum);
static struct vnode *_find_vnode(pid_t pid, procfs_filenum_t filenum, mode_t mode, struct mount *mp);
static struct vnode *_alloc_vnode(pid_t pid, procfs_filenum_t filenum, mode_t mode, struct mount *mp);

int procfs_init()
{
	for (short i = 0; i < MAX_VNODES; i++)
		vnode_table[i].vn.refcount = 0;
	return 0;
}

int procfs_mount(struct mount *mp, device_t dev, struct vnode *parent)
{
	mp->root_node = _alloc_vnode(0, 0, S_IFDIR | 0755, mp);
	return 0;
}

int procfs_unmount(struct mount *mp)
{
	vfs_release_vnode(mp->root_node);
	return 0;
}

int procfs_lookup(struct vnode *vnode, const char *filename, struct vnode **result)
{
	pid_t pid = 0;
	procfs_filenum_t filenum;
	struct process *proc;

	if (PROCFS_DATA(vnode).filenum == PFN_ROOTDIR) {
		if (*filename >= '0' && *filename <= '9') {
			pid = strtol(filename, NULL, 10);
			filenum = PFN_PROCDIR;

			proc = get_proc(pid);
			if (!proc)
				return ENOENT;
		}
		else {
			filenum = _find_filenum_by_name(root_files, filename);
			if (filenum < 0)
				return ENOENT;
		}
	}
	else if (PROCFS_DATA(vnode).filenum == PFN_PROCDIR) {
		pid = PROCFS_DATA(vnode).pid;

		filenum = _find_filenum_by_name(proc_files, filename);
		if (filenum < 0)
			return ENOENT;
	}
	else
		return ENOTDIR;

	if (*result)
		vfs_release_vnode(*result);
	*result = _find_vnode(pid, filenum, S_IFDIR | 0755, vnode->mp);
	return 0;
}

int procfs_release(struct vnode *vnode)
{
	return 0;
}

int procfs_open(struct vfile *file, int flags)
{
	if (PROCFS_DATA(file->vnode).filenum == PFN_ROOTDIR) {
		PROCFS_POSITION(file->position)->slot = 0;
		proc_iter_start(&PROCFS_POSITION(file->position)->iter);
	}
	else
		file->position = 0;
	return 0;
}

int procfs_close(struct vfile *file)
{
	return 0;
}

int procfs_read(struct vfile *file, char *buf, size_t nbytes)
{
	int limit = 0;
	struct process *proc;
	char buffer[MAX_BUFFER];
	struct procfs_dir_entry *entry;

	proc = get_proc(PROCFS_DATA(file->vnode).pid);
	if (PROCFS_DATA(file->vnode).pid != 0 && !proc)
		return ENOENT;

	entry = _get_entry_by_num(proc_files, PROCFS_DATA(file->vnode).filenum);
	if (!entry)
		entry = _get_entry_by_num(root_files, PROCFS_DATA(file->vnode).filenum);
	if (entry)
		limit = entry->func(proc, buffer, MAX_BUFFER);

	if (file->position + nbytes >= limit)
		nbytes = limit - file->position;
	if (nbytes)
		strncpy(buf, &buffer[file->position], nbytes);
	file->position += nbytes;
	return nbytes;
}

int procfs_write(struct vfile *file, const char *buf, size_t nbytes)
{
	return 0;
}

int procfs_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid)
{
	return -1;
}

offset_t procfs_seek(struct vfile *file, offset_t position, int whence)
{
	return -1;
}

#define PROCFS_MAX_PID		0x10000

int procfs_readdir(struct vfile *file, struct dirent *dir)
{
	int i;
	short slot;
	struct process *proc;

	if (!S_ISDIR(file->vnode->mode))
		return ENOTDIR;

	if (PROCFS_DATA(file->vnode).filenum == PFN_ROOTDIR) {
		if (PROCFS_POSITION(file->position)->slot == 0 && (proc = proc_iter_next(&PROCFS_POSITION(file->position)->iter))) {
			dir->d_ino = file->position;
			snprintf(dir->d_name, VFS_FILENAME_MAX, "%d", proc->pid);
			dir->d_name[VFS_FILENAME_MAX - 1] = '\0';
		}
		else {
			slot = ++PROCFS_POSITION(file->position)->slot;

			if (!root_files[slot - 1].filename)
				return 0;

			dir->d_ino = file->position;
			strncpy(dir->d_name, root_files[slot - 1].filename, VFS_FILENAME_MAX);
			dir->d_name[VFS_FILENAME_MAX - 1] = '\0';
		}
	}
	else if (PROCFS_DATA(file->vnode).filenum == PFN_PROCDIR) {
		if (!proc_files[file->position].filename)
			return 0;
		dir->d_ino = (PROCFS_DATA(file->vnode).pid << 8) | proc_files[file->position].filenum;
		strncpy(dir->d_name, proc_files[file->position].filename, VFS_FILENAME_MAX);
		dir->d_name[VFS_FILENAME_MAX - 1] = '\0';
		file->position += 1;
	}
	else
		return ENOTDIR;
	return 1;
}


static procfs_filenum_t _find_filenum_by_name(struct procfs_dir_entry *entries, const char *filename)
{
	procfs_filenum_t filenum;

	for (short i = 0; entries[i].filename; i++) {
		if (!strcmp(filename, entries[i].filename)) {
			filenum = entries[i].filenum;
			return filenum;
		}
	}
	return ENOENT;
}

static struct procfs_dir_entry *_get_entry_by_num(struct procfs_dir_entry *entries, procfs_filenum_t filenum)
{
	for (short i = 0; entries[i].filename; i++) {
		if (entries[i].filenum == filenum)
			return &entries[i];
	}
	return NULL;
}

static struct vnode *_find_vnode(pid_t pid, procfs_filenum_t filenum, mode_t mode, struct mount *mp)
{
	for (short i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].vn.refcount > 0 && PROCFS_DATA(&vnode_table[i]).pid == pid && PROCFS_DATA(&vnode_table[i]).filenum == filenum)
			return vfs_clone_vnode(&vnode_table[i].vn);
	}
	return _alloc_vnode(pid, filenum, mode, mp);
}

static struct vnode *_alloc_vnode(pid_t pid, procfs_filenum_t filenum, mode_t mode, struct mount *mp)
{
	for (short i = 0; i < MAX_VNODES; i++) {
		if (vnode_table[i].vn.refcount <= 0) {
			vfs_init_vnode(&vnode_table[i].vn, &procfs_vnode_ops, mp, mode, 1, 0, 0, 0, filenum, 0, 0, 0, 0);
			PROCFS_DATA(&vnode_table[i]).pid = pid;
			PROCFS_DATA(&vnode_table[i]).filenum = filenum;
			return &vnode_table[i].vn;
		}
	}
	return NULL;
}

