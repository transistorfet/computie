

#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/syscall.h>


#include "process.h"
#include "interrupts.h"


#define TASK_SIZE	800
const char hello_task[TASK_SIZE] = {
#include "../test.txt"
};


extern struct process *current_proc;
extern void *current_proc_stack;
extern void *create_context(void *user_stack, void *entry);

/*
int test_process()
{
	int error = 0;

	DISABLE_INTS();

	struct process *proc = new_proc(SU_UID);
	if (!proc) {
		printk("Ran out of procs\n");
		return NULL;
	}

	current_proc = proc;

	int fd = do_open("tty", 0, 0);
	if (fd < 0) {
		printk("Error opening file tty %d\n", error);
		return NULL;
	}
	printk("FD: %d\n", fd);

	do_write(fd, "Hey\n", 4);


	//for (int i = 0; i < 0x4000; i++)
	//	asm volatile("");

	int task_size = 0x1800;
	char *task = kmalloc(task_size);
	char *task_stack_p = task + task_size;
	printk("Task Address: %x\n", task);
	printk("Task Stack: %x\n", task_stack_p);

	//memset_s(task, 0, 0xC00);		// With memset doing nothing, this value will not cause a fatal
	//memset_s(task, 0, 0xD00);		// With memset doing nothing, this value will sometimes cause a fatal
	//memset_s(task, 0, 0xF00);		// With memset doing nothing, this value will mostly cause a fatal

	//memset_s(task + 0x400, 0, 0x1000);	// TODO this will cause the fatal and the string glitch
	//memset_s(task + 0x800, 0, 0xB00);	// Works but causes the string glitch
	//memset_s(task + 0x400, 0, 0xB00);	// Works but causes the string glitch

	//memset_s(task, 0, task_size);
	memcpy_s(task, hello_task, TASK_SIZE);
	//dump(task, task_size);

	//print_stack();

 	task_stack_p = create_context(task_stack_p, task);
 	//task_stack_p = create_context(task_stack_p, exit2);

	//dump(task_stack_p, 0x40);

	proc->map.segments[M_TEXT].base = task;
	proc->map.segments[M_TEXT].length = task_size;
	proc->sp = task_stack_p;

	printk("After: %x\n", task_stack_p);

	ENABLE_INTS();

	return proc;
}
*/


void test_files()
{
	int fd;
	int error;
	struct vfile *file;

	error = vfs_open(NULL, "/dir", O_CREAT, S_IFDIR | 0755, SU_UID, &file);
	if (error) {
		printk("Error: %d\n", error);
		return;
	}
	vfs_close(file);

	error = vfs_open(NULL, "/dir/test", O_CREAT, 0644, SU_UID, &file);
	if (error) {
		printk("Error: %d\n", error);
		return;
	}
	vfs_close(file);

/*
	fd = creat("/dir", S_IFDIR | 0755);
	if (fd < 0) {
		printk("Error: %d\n", fd);
		return;
	}
	close(fd);

	fd = creat("/dir/test", 0644);
	if (fd < 0) {
		printk("Error: %d\n", fd);
		return;
	}
	close(fd);
*/

	if ((error = vfs_open(NULL, "test", O_CREAT, 0755, SU_UID, &file))) {
		printk("Error at open %d\n", error);
		return;
	}

	if ((error = vfs_write(file, "This is a file test\n", 20)) <= 0) {
		printk("Error when writing %d\n", error);
		return;
	}

	vfs_seek(file, 0, 0);

	char buffer[256];

	error = vfs_read(file, buffer, 256);
	if (error < 0) {
		printk("Error when reading\n");
		return;
	}
	printk("Read: %d\n", error);
	buffer[error] = '\0';

	puts(buffer);

	vfs_close(file);


	extern const char hello_task[800];
	if ((error = vfs_open(NULL, "/hello", O_CREAT, 0755, SU_UID, &file))) {
		printk("Error at open new file %d\n", error);
		return;
	}

	if ((error = vfs_write(file, hello_task, 256)) <= 0) {
		printk("Error when writing %d\n", error);
		return;
	}

	vfs_close(file);

	puts("done");

	if ((error = vfs_open(NULL, "/size", O_CREAT, 0644, SU_UID, &file))) {
		printk("Error at open new file %d\n", error);
		return;
	}

	char data[2];
	for (short i = 0; i < 100; i++) {
		for (short j = 0; j < 10; j++) {
			data[0] = '0' + j;
			//printk("%d\n", data[0]);
			if ((error = vfs_write(file, data, 1)) <= 0) {
				printk("Error when writing %d\n", error);
				return;
			}
		}
	}

	vfs_close(file);


	if (access("/dir/test", R_OK) == 0)
		printk("Readable\n");
	else
		printk("Not readable\n");


	error = rename("size", "dir/size");
	if (error) {
		printk("Error renaming: %d\n", error);
		return;
	}

	//if (chown("hello", 2, 0)) {
	//	printk("Error changing owner: %d\n", error);
	//}

}

void test_dirs()
{
	int error;
	struct vfile *file;
	struct vdir dir;

	if ((error = vfs_open(NULL, "/", 0, 0, SU_UID, &file))) {
		printk("Error at open %d\n", error);
		return;
	}

	while (1) {
		error = vfs_readdir(file, &dir);
		if (error < 0) {
			printk("Error at readdir %d\n", error);
			return;
		}
		else if (error == 0)
			break;
		else {
			printk("File: %d:%s\n", error, dir.name);
		}
	}

	vfs_close(file);
}


void test_fork()
{
	//int pid = SYSCALL1(SYS_FORK, 0);
	int pid = fork();

	if (pid) {
		int status;

		printf("The child's pid is %d\n", pid);
		wait(&status);
		printf("The child exited with %d\n", status);
	}
	else {
		puts("I AM THE CHILD!");
		exit(1337);
	}
}

void test_pipe()
{
	int error;
	int pipes[2];
	char buffer[50];

	error = pipe(pipes);
	if (error) {
		printf("Pipe failed with error %d\n", error);
		return;
	}

	error = write(pipes[PIPE_WRITE_FD], "Hey there, this is a pipe|\n", 27);
	if (error < 0) {
		printf("Failed to write to pipe: %d\n", error);
		return;
	}
	printf("Wrote %d bytes\n", error);

	error = read(pipes[PIPE_READ_FD], buffer, 50);
	if (error < 0) {
		printf("Failed to read to pipe: %d\n", error);
		return;
	}
	printf("Read %d bytes\n", error);
	buffer[error] = '\0';

	printf("> %s\n", buffer);

	close(pipes[PIPE_READ_FD]);
	close(pipes[PIPE_WRITE_FD]);

	return;
}

void test_forkpipe()
{
	int pid;
	int error;
	int pipes[2];
	char buffer[50];

	error = pipe(pipes);
	if (error) {
		printf("Pipe failed with error %d\n", error);
		return;
	}

 	pid = fork();
	if (pid) {
		close(pipes[PIPE_WRITE_FD]);

		error = read(pipes[PIPE_READ_FD], buffer, 50);
		if (error < 0) {
			printf("Failed to read to pipe: %d\n", error);
			return;
		}
		printf("Read %d bytes\n", error);
		buffer[error] = '\0';

		printf("> %s\n", buffer);

		close(pipes[PIPE_READ_FD]);

	} else {
		close(pipes[PIPE_READ_FD]);

		error = write(pipes[PIPE_WRITE_FD], "Hey there, this is a pipe|\n", 27);
		if (error < 0) {
			printf("Failed to write to pipe: %d\n", error);
			return;
		}
		printf("Wrote %d bytes\n", error);

		close(pipes[PIPE_WRITE_FD]);
		exit(0);
	}

	return;
}

