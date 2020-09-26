
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/driver.h>
#include <kernel/kmalloc.h>

#include "proc/memory.h"
#include "proc/process.h"

#include "api.h"
#include "filedesc.h"
#include "interrupts.h"


#define SYSCALL_MAX	32

void test() { printk("It's a test!\n"); }

// TODO remove this after testing
int do_execbuiltin(void *addr, char *const argv[], char *const envp[]);

void *syscall_table[SYSCALL_MAX] = {
	test,
	do_exit,
	do_fork,
	do_read,
	do_write,
	do_open,
	do_close,
	do_wait,
	do_creat,
	test,		// 9 = link, not yet implemented
	do_unlink,
	do_exec,
	do_stat,
	do_fstat,
	do_lseek,
	do_waitpid,
	do_pipe,
	do_readdir,
	do_mkdir,
	do_chdir,
	do_chmod,
	do_chown,
	do_access,
	do_rename,
	do_dup2,

	do_execbuiltin,
};

extern void enter_syscall();

void init_syscall()
{
	set_interrupt(IV_TRAP1, enter_syscall);
}

// TODO this is temporary until you have processes working correctly
extern struct process *current_proc;
extern struct syscall_record *current_syscall;


void tty_68681_set_leds(uint8_t bits);
void tty_68681_reset_leds(uint8_t bits);

//
// Perform a system call and pass the return value to the calling process
//
void do_syscall()
{
	int ret;

	tty_68681_set_leds(0x04);

	backup_current_proc();
	ret = ((syscall_t) syscall_table[current_syscall->syscall])(current_syscall->arg1, current_syscall->arg2, current_syscall->arg3);
	return_to_current_proc(ret);

	tty_68681_reset_leds(0x04);
}

/*
int do_mount(const char *source, const char *target)
{
	struct mount *mp;
	struct vnode *vnode;

	if (vfs_lookup(current_proc->cwd, source, VLOOKUP_NORMAL, current_proc->uid, &vnode))
		return ENOENT;

	vfs_mount(current_proc->cwd, target, DEVNUM(DEVMAJOR_MEM, 0), &minix_mount_ops, current_proc->uid, &mp);
}
*/

int do_unlink(const char *path)
{
	return vfs_unlink(current_proc->cwd, path, current_proc->uid);
}

int do_rename(const char *oldpath, const char *newpath)
{
	return vfs_rename(current_proc->cwd, oldpath, newpath, current_proc->uid);
}

int do_mkdir(const char *path, mode_t mode)
{
	int error;
	struct vfile *file;

	error = vfs_open(current_proc->cwd, path, O_CREAT, S_IFDIR | mode, current_proc->uid, &file);
	if (error < 0)
		return error;
	vfs_close(file);
	return 0;
}

int do_creat(const char *path, mode_t mode)
{
	return do_open(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int do_open(const char *path, int oflags, mode_t mode)
{
	int fd;
	int error;
	struct vfile *file;

	fd = find_unused_fd(current_proc->fd_table);
	if (fd < 0)
		return fd;

	error = vfs_open(current_proc->cwd, path, oflags, mode, current_proc->uid, &file);
	if (error)
		return error;

	set_fd(current_proc->fd_table, fd, file);

	return fd;
}

int do_close(int fd)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;

	vfs_close(file);

	unset_fd(current_proc->fd_table, fd);
	//free_fd(current_proc->fd_table, fd);
	return 0;
}

size_t do_read(int fd, char *buf, size_t nbytes)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_read(file, buf, nbytes);
}


size_t do_write(int fd, const char *buf, size_t nbytes)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_write(file, buf, nbytes);
}

int do_lseek(int fd, offset_t offset, int whence)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_seek(file, offset, whence);
}

int do_readdir(int fd, struct dirent *dir)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_readdir(file, dir);
}

int do_chdir(const char *path)
{
	int error;
	struct vnode *vnode;

	if ((error = vfs_lookup(current_proc->cwd, path, VLOOKUP_NORMAL, current_proc->uid, &vnode)))
		return error;

	if (!(vnode->mode & S_IFDIR))
		return ENOTDIR;

	current_proc->cwd = vfs_clone_vnode(vnode);
	return 0;
}

int do_access(const char *path, int mode)
{
	return vfs_access(current_proc->cwd, path, mode, current_proc->uid);
}

int do_chmod(const char *path, int mode)
{
	return vfs_chmod(current_proc->cwd, path, mode, current_proc->uid);
}

int do_chown(const char *path, uid_t owner, gid_t group)
{
	return vfs_chown(current_proc->cwd, path, owner, group, current_proc->uid);
}

int do_stat(const char *path, struct stat *statbuf)
{
	int error;
	struct vnode *vnode;

	if ((error = vfs_lookup(current_proc->cwd, path, VLOOKUP_NORMAL, current_proc->uid, &vnode)))
		return error;

	statbuf->st_dev = 0;
	statbuf->st_mode = vnode->mode;
	statbuf->st_nlinks = vnode->nlinks;
	statbuf->st_uid = vnode->uid;
	statbuf->st_gid = vnode->gid;
	statbuf->st_size = vnode->size;
	statbuf->st_atime = vnode->atime;
	statbuf->st_mtime = vnode->mtime;
	statbuf->st_ctime = vnode->ctime;

	vfs_release_vnode(vnode);
	return 0;
}

int do_fstat(int fd, struct stat *statbuf)
{
	struct vfile *file;

	file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;

	statbuf->st_dev = 0;
	statbuf->st_mode = file->vnode->mode;
	statbuf->st_nlinks = file->vnode->nlinks;
	statbuf->st_uid = file->vnode->uid;
	statbuf->st_gid = file->vnode->gid;
	statbuf->st_size = file->vnode->size;
	statbuf->st_atime = file->vnode->atime;
	statbuf->st_mtime = file->vnode->mtime;
	statbuf->st_ctime = file->vnode->ctime;

	return 0;
}

#define PIPE_READ_FD	0
#define PIPE_WRITE_FD	1

int do_pipe(int pipefd[2])
{
	int error;
	struct vfile *file[2];

	if (!pipefd)
		return EINVAL;

	// Find two unused file descriptor slots in the current process's fd table (no need to free them until set_fd())
	pipefd[PIPE_READ_FD] = find_unused_fd(current_proc->fd_table);
	// TODO this is terrible and I shouldn't do this, but at least it works for the time being
	set_fd(current_proc->fd_table, pipefd[PIPE_READ_FD], 1);
	pipefd[PIPE_WRITE_FD] = find_unused_fd(current_proc->fd_table);
	set_fd(current_proc->fd_table, pipefd[PIPE_READ_FD], NULL);

	if (pipefd[PIPE_READ_FD] < 0 || pipefd[PIPE_WRITE_FD] < 0)
		return EMFILE;

	error = vfs_create_pipe(&file[PIPE_READ_FD], &file[PIPE_WRITE_FD]);
	if (error)
		return error;

	set_fd(current_proc->fd_table, pipefd[PIPE_READ_FD], file[PIPE_READ_FD]);
	set_fd(current_proc->fd_table, pipefd[PIPE_WRITE_FD], file[PIPE_WRITE_FD]);

	return 0;
}

int do_dup2(int oldfd, int newfd)
{
	struct vfile *fileptr, *exfileptr;

	fileptr = get_fd(current_proc->fd_table, oldfd);

	if (!fileptr)
		return EBADF;

	if (newfd >= OPEN_MAX)
		return EBADF;

	exfileptr = get_fd(current_proc->fd_table, newfd);
	if (exfileptr)
		vfs_close(exfileptr);

	dup_fd(current_proc->fd_table, newfd, fileptr);
	return 0;
}


pid_t do_fork()
{
	struct process *proc;

	proc = new_proc(current_proc->uid);
	if (!proc)
		panic("Ran out of procs\n");
	proc->cwd = current_proc->cwd;
	dup_fd_table(proc->fd_table, current_proc->fd_table);

	clone_stack(current_proc, proc);

	// Apply return values to the context on the stack
	set_proc_return_value(proc, 0);
	//set_proc_return_value(current_proc, proc->pid);

	return proc->pid;
}

void do_exit(int exitcode)
{
	exit_proc(current_proc, exitcode);
	resume_waiting_parent(current_proc);
}

pid_t do_wait(int *status)
{
	return do_waitpid(-1, status, 0);
}

pid_t do_waitpid(pid_t pid, int *status, int options)
{
	struct process *proc;

	// Must be a valid pid, or -1 to wait for any process
	if (pid <= 0 && pid != -1)
		return EINVAL;

	proc = find_exited_child(current_proc->pid, pid);
	if (!proc) {
		suspend_current_proc();
	}
	else {
		*status = proc->exitcode;
		pid = proc->pid;
		cleanup_proc(proc);
		return pid;
	}
}

int do_exec(const char *path, char *const argv[], char *const envp[])
{
	int error;

	error = load_flat_binary(path, &current_proc->map);
	if (error)
		return error;

	reset_stack(current_proc, current_proc->map.segments[M_TEXT].base, argv, envp);

	return 0;
}

int do_execbuiltin(void *addr, char *const argv[], char *const envp[])
{
	//if (current_proc->map.segments[M_TEXT].base)
	//	kmfree(current_proc->map.segments[M_TEXT].base);
	current_proc->map.segments[M_TEXT].base = NULL;
	current_proc->map.segments[M_TEXT].length = 0;

	reset_stack(current_proc, addr, argv, envp);

	return 0;
}

