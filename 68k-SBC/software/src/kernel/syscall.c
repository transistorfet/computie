
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/driver.h>
#include <kernel/filedesc.h>

#include "api.h"
#include "process.h"
#include "interrupts.h"

#define SYSCALL_MAX	20

typedef int (*syscall_t)(int, int, int);

void test() { puts("It's a test!\n"); }

void *syscall_table[SYSCALL_MAX] = {
	test,
	do_exit,
	do_fork,
	do_read,
	do_write,
	do_open,
	do_close,
	do_wait,
	do_readdir,
	test,		// 9 = link, not yet implemented
	do_unlink,
	do_exec,
	do_stat,
	do_fstat,
	do_lseek,
	do_waitpid,
	do_pipe,
};

extern void enter_syscall();

void init_syscall()
{
	set_interrupt(IV_TRAP1, enter_syscall);
}

// TODO this is temporary until you have processes working correctly
extern struct process *current_proc;
extern void *current_proc_stack;
extern struct syscall_record *current_syscall;


//
// Perform a system call and pass the return value to the calling process
//
void do_syscall()
{
	int ret;
	ret = ((syscall_t) syscall_table[current_syscall->syscall])(current_syscall->arg1, current_syscall->arg2, current_syscall->arg3);
	if (current_proc->state == PS_READY) {
		// If the process is still in the ready state, then set the return value in the process's context
		*((uint32_t *) current_proc_stack) = ret;
	}
	else {
		// If the process has been suspended, we wont return as normal but instead schedule another process
		schedule();
	}
}


int do_open(const char *path, int oflags)
{
	int fd;
	int error;
	struct vfile *file;

	fd = find_unused_fd(current_proc->fd_table);
	if (fd < 0)
		return fd;

	error = vfs_open(path, oflags, &file);
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

	free_fileptr(file);
	free_fd(current_proc->fd_table, fd);
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

int do_readdir(int fd, struct vdir *dir)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_readdir(file, dir);
}


pid_t do_fork()
{
	struct process *proc;

	proc = new_proc();
	if (!proc)
		panic("Ran out of procs\n");
	dup_fd_table(proc->fd_table, current_proc->fd_table);

	// Save the current process's stack pointer back to it's struct, which
	// normally only happens in the schedule() function
	current_proc->sp = current_proc_stack;

	int stack_size = current_proc->segments[M_STACK].length;
	char *stack = malloc(stack_size);
	char *stack_pointer = (stack + stack_size) - ((current_proc->segments[M_STACK].base + current_proc->segments[M_STACK].length) - current_proc->sp);

	memcpy_s(stack, current_proc->segments[M_STACK].base, current_proc->segments[M_STACK].length);

	printf("Parent Stack Pointer: %x\n", current_proc->sp);

	printf("Fork Stack: %x\n", stack);
	printf("Fork Stack Top: %x\n", stack + stack_size);
	printf("Fork Stack Pointer: %x\n", stack_pointer);

	proc->segments[M_STACK].base = stack;
	proc->segments[M_STACK].length = stack_size;
	proc->sp = stack_pointer;

	// Apply return values to the context on the stack (%d0 is at the top)
	*((uint32_t *) current_proc->sp) = proc->pid;
	*((uint32_t *) proc->sp) = 0;

	//dump((uint16_t *) current_proc->sp, 0x100);
	//dump((uint16_t *) proc->sp, 0x100);

	printf("PID: %d\n", proc->pid);

	return proc->pid;
}

void do_exit(int exitcode)
{
	struct process *parent;

	exit_proc(current_proc, exitcode);

	parent = get_proc(current_proc->parent);
	if (parent->state == PS_BLOCKED && (parent->blocked_call.syscall == SYS_WAIT || parent->blocked_call.syscall == SYS_WAITPID))
		resume_proc(parent);
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

int do_exec(const char *path)
{
	int fd;
	int error;
	struct stat statbuf;

	if ((error = do_stat(path, &statbuf))) {
		printf("Error stating %s: %d\n", path, error);
		return error;
	}

	if ((fd = do_open(path, O_RDONLY)) < 0) {
		printf("Error opening %s: %d\n", path, fd);
		return fd;
	}

	printf("Size: %d\n", statbuf.st_size);

	// The extra data is for the bss segment
	int task_size = statbuf.st_size + 0x100;
	//int task_size = 0x1000;
	char *task_text = malloc(task_size);

	error = do_read(fd, task_text, task_size);
	do_close(fd);


	if (error < 0) {
		printf("Error reading file %s: %d\n", path, error);
		return error;
	}

	// TODO overwriting this could be a memory leak.  How do I refcount segments?
	current_proc->segments[M_TEXT].base = task_text;
	current_proc->segments[M_TEXT].length = task_size;

	// Reset the stack to start our new process
	char *task_stack_pointer = current_proc->segments[M_TEXT].base + current_proc->segments[M_TEXT].length;
 	task_stack_pointer = create_context(task_stack_pointer, task_text);
	current_proc->sp = task_stack_pointer;
	current_proc_stack = task_stack_pointer;

	printf("Exec Text: %x\n", task_text);
	printf("Exec Stack Pointer: %x\n", task_stack_pointer);

	//dump((uint16_t *) task_text, 400);

	return 0;
}

int do_unlink(const char *path)
{
	return vfs_unlink(path);
}

int do_stat(const char *path, struct stat *statbuf)
{
	int error;
	struct vnode *vnode;

	if ((error = vfs_lookup(path, VLOOKUP_NORMAL, &vnode)))
		return error;

	statbuf->st_dev = 0;
	statbuf->st_mode = vnode->mode;
	statbuf->st_uid = vnode->uid;
	statbuf->st_gid = vnode->gid;
	statbuf->st_size = vnode->size;

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
	statbuf->st_uid = file->vnode->uid;
	statbuf->st_gid = file->vnode->gid;
	statbuf->st_size = file->vnode->size;

	return 0;
}

int do_lseek(int fd, offset_t offset, int whence)
{
	struct vfile *file = get_fd(current_proc->fd_table, fd);
	if (!file)
		return EBADF;
	return vfs_seek(file, offset, whence);
}

int do_pipe(int pipefd[2])
{
	// TODO implement pipe
	return -1;
}

