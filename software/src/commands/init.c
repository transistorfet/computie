
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioc_tty.h>
#include <kernel/syscall.h>

int init_task()
{
	// Open stdin
	int fd = open("/dev/tty0", O_RDONLY, 0);
	if (fd < 0) {
		printf("Error opening file tty %d\n", fd);
		return NULL;
	}
	// Open stdout
	dup2(fd, 1);
	// Open stderr
	dup2(fd, 2);


	// TODO temporary, for testing
	int test_files();
	int test_dirs();
	//test_files();
	//test_dirs();


	int pid, status;
	char *argv[2] = { NULL };
	char *envp[2] = { NULL };

 	pid = fork();
	if (!pid) {
		setpgid(0, 0);
		pid_t fgpid = getpgid(0);
		ioctl(STDOUT_FILENO, TIOCSPGRP, &fgpid);

		extern void sh_task();
		argv[0] = "sh";
		status = SYSCALL3(SYS_EXECBUILTIN, (int) sh_task, (int) argv, (int) envp);
		// The exec() system call will only return if an error occurs
		printf("Failed to execute %s: %d\n", argv[1], status);
		exit(-1);
	}

	while (1) {
		wait(&status);
		printf("Process %d exited with %d\n", pid, status);
	}
}


