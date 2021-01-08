
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_ln)(int argc, char **argv, char **envp)
{
	if (argc <= 2) {
		puts("Usage: ln <source> <target>");
		return -1;
	}

	int error = link(argv[1], argv[2]);
	if (error < 0) {
		printf("Error while linking %s to %s: %d\n", argv[1], argv[2], error);
		return error;
	}

	return 0;
}
