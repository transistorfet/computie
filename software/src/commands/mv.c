
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_mv)(int argc, char **argv, char **envp)
{
	if (argc <= 2) {
		puts("Usage: mv <file> <dest>");
		return -1;
	}

	int error = rename(argv[1], argv[2]);
	if (error < 0) {
		printf("Error while renaming %s: %d\n", argv[1], error);
		return error;
	}

	return 0;
}
