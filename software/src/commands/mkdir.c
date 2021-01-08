
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_mkdir)(int argc, char **argv, char **envp)
{
	if (argc <= 1) {
		puts("Usage: mkdir <dir>");
		return -1;
	}

	int error = mkdir(argv[1], 0755);
	if (error < 0) {
		printf("Error while making directory %s: %d\n", argv[1], error);
	}

	return 0;
}
