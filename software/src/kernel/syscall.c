
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ioc_tty.h>
#include <kernel/vfs.h>
#include <kernel/time.h>
#include <kernel/signal.h>
#include <kernel/printk.h>
#include <kernel/driver.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>

// TODO these should maybe be bundled into an interface in <kernel/processes.h>
#include "proc/timer.h"
#include "proc/memory.h"
#include "proc/process.h"
#include "proc/binaries.h"
#include "proc/filedesc.h"

#include "api.h"
#include "interrupts.h"


#define SYSCALL_MAX	48

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
	do_link,
	do_unlink,
	do_waitpid,
	do_chdir,
	do_time,
	do_mknod,
	do_chmod,
	do_chown,
	do_stat,
	do_lseek,
	do_getpid,
	do_mount,
	do_umount,
	do_getuid,
	do_fstat,
	do_access,
	do_sync,
	do_kill,
	do_rename,
	do_mkdir,
	do_dup2,
	do_pipe,
	do_ioctl,
	do_exec,
	do_readdir,
	do_getppid,
	test,		// 35 = symlink, not yet implemented
	do_getpgid,
	do_setpgid,
	do_alarm,
	do_pause,
	do_sigreturn,
	do_sigaction,
	do_stime,

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

	ret = ((syscall_t) syscall_table[current_syscall->syscall])(current_syscall->arg1, current_syscall->arg2, current_syscall->arg3);
	return_to_current_proc(ret);

	tty_68681_reset_leds(0x04);
}

int do_mount(const char *source, const char *target, struct mount_opts *opts)
{
	extern struct mount_ops **filesystems;
	extern struct mount_ops minix_mount_ops;

	struct vnode *vnode;
	struct mount_ops *fsptr = NULL;

	if (opts && opts->fstype) {
		for (char i = 0; filesystems[i]; i++) {
			if (!strcmp(filesystems[i]->fstype, opts->fstype)) {
				fsptr = filesystems[i];
				break;
			}
		}
		if (!fsptr)
			return EINVAL;
	}
	if (!fsptr)
		fsptr = &minix_mount_ops;

	if (vfs_lookup(current_proc->cwd, source, VLOOKUP_NORMAL, current_proc->uid, &vnode))
		return ENOENT;
	return vfs_mount(current_proc->cwd, target, vnode->rdev, fsptr, opts ? opts->mountflags : 0, current_proc->uid);
}

int do_umount(const char *source)
{
	struct vnode *vnode;

	if (vfs_lookup(current_proc->cwd, source, VLOOKUP_NORMAL, current_proc->uid, &vnode))
		return ENOENT;

	return vfs_unmount(vnode->rdev, current_proc->uid);
}

int do_sync()
{
	return vfs_sync(0);
}

int do_mknod(const char *path, mode_t mode, device_t dev)
{
	int error;
	struct vnode *vnode;

	error = vfs_mknod(current_proc->cwd, path, mode, dev, current_proc->uid, &vnode);
	if (error)
		return error;
	vfs_release_vnode(vnode);
	return 0;
}

int do_link(const char *oldpath, const char *newpath)
{
	return vfs_link(current_proc->cwd, oldpath, newpath, current_proc->uid);
}

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

	mode = mode & ~current_proc->umask & 0777;
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

	mode = mode & ~current_proc->umask & 0777;
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

int do_ioctl(int fd, unsigned int request, void *argp)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_ioctl(file, request, argp);
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

	statbuf->st_dev = vnode->mp->dev;
	statbuf->st_mode = vnode->mode;
	statbuf->st_nlinks = vnode->nlinks;
	statbuf->st_uid = vnode->uid;
	statbuf->st_gid = vnode->gid;
	statbuf->st_rdev = vnode->rdev;
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
	statbuf->st_rdev = file->vnode->rdev;
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

	proc = new_proc(0, current_proc->uid);
	if (!proc)
		panic("Ran out of procs\n");

	clone_process_memory(current_proc, proc);

	// Apply return value to the stack context of the cloned proc, and return to the parent with the new pid
	set_proc_return_value(proc, 0);
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

	// TODO should this wake up all parent procesess instead of just one?
	proc = find_exited_child(current_proc->pid, pid);
	if (!proc) {
		current_proc->bits |= PB_WAITING;
		suspend_current_proc();
	}
	else {
		current_proc->bits &= ~PB_WAITING;
		*status = proc->exitcode;
		pid = proc->pid;
		cleanup_proc(proc);
		return pid;
	}
}

pid_t do_getpid()
{
	return current_proc->pid;
}

pid_t do_getppid()
{
	return current_proc->parent;
}

pid_t do_getpgid(pid_t pid)
{
	struct process *proc = pid ? get_proc(pid) : current_proc;

	if (!proc)
		return ESRCH;
	return proc->pgid;
}

int do_setpgid(pid_t pid, pid_t pgid)
{
	struct process *proc = pid ? get_proc(pid) : current_proc;

	if (pgid < 0)
		return EINVAL;
	if (!proc || (proc != current_proc && proc->pid != current_proc->parent))
		return ESRCH;

	if (pgid == 0)
		proc->pgid = proc->pid;
	else {
		struct process *pg;

		pg = get_proc(pgid);
		if (!pg || pg->session != proc->session || proc->pid == proc->session)
			return EPERM;
		proc->pgid = pgid;
	}
	return 0;
}

uid_t do_getuid()
{
	return current_proc->uid;
}

int do_kill(pid_t pid, int sig)
{
	if (pid < 0)
		return send_signal_process_group(-pid, sig);
	else
		return send_signal(pid, sig);
}

unsigned int do_alarm(unsigned int seconds)
{
	return set_alarm(current_proc, seconds);
}

int do_pause()
{
	current_proc->bits |= PB_PAUSED;
	suspend_current_proc();
	return 0;
}

int do_sigreturn()
{
	cleanup_signal_handler();
	return 0;
}

int do_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	if (oldact)
		get_signal_action(current_proc, signum, oldact);

	if (act)
		return set_signal_action(current_proc, signum, act);
	return 0;
}

int do_exec(const char *path, char *const argv[], char *const envp[])
{
	int error;
	void *entry;

	error = load_binary(path, current_proc, &entry);
	if (error == EKILL) {
		// An error occurred past the point of no return.  The memory maps have been irrepairably damaged, so kill the process
		printk_safe("Process terminated\n");
		exit_proc(current_proc, -1);
		resume_waiting_parent(current_proc);
		return error;
	}
	else if (error)
		return error;

	reset_stack(current_proc, entry, argv, envp);

	return 0;
}

int do_execbuiltin(void *addr, char *const argv[], char *const envp[])
{
	// NOTE no modification of the memory maps here, since the code should be in the same process
	reset_stack(current_proc, addr, argv, envp);
	return 0;
}

time_t do_time(time_t *t)
{
	time_t current = get_system_time();
	if (t)
		*t = current;
	return current;
}

int do_stime(const time_t *t)
{
	if (current_proc->uid != SU_UID)
		return EPERM;
	set_system_time(*t);
	return 0;
}

