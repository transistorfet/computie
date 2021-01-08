
#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>

#include "prototype.h"

int MAIN(command_umount)(int argc, char **argv, char **envp)
{
	int error;

	if (argc <= 1) {
		puts("Usage: umount <devfile>");
		return -1;
	}

	error = umount(argv[1]);
	if (error < 0) {
		printf("Error while unmounting %d\n", error);
		return -1;
	}

	return 0;
}


