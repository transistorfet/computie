
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "prototype.h"

int MAIN(command_mount)(int argc, char **argv, char **envp)
{
	int error;

	if (argc <= 2) {
		puts("Usage: mount <devfile> <mountpoint>");
		return -1;
	}

	error = mount(argv[1], argv[2], NULL);
	if (error < 0) {
		printf("Error while mounting %d\n", error);
		return -1;
	}

	return 0;
}


