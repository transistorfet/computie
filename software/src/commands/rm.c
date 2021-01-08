
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_rm)(int argc, char **argv, char **envp)
{
	if (argc <= 1) {
		puts("Usage: rm <file>");
		return -1;
	}

	int error = unlink(argv[1]);
	if (error < 0) {
		printf("Error while unlinking %s: %d\n", argv[1], error);
	}

	return 0;
}

