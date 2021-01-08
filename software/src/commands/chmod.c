
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_chmod)(int argc, char **argv, char **envp)
{
	if (argc <= 2) {
		puts("Usage: chmod <mode> <file>");
		return -1;
	}

	for (int i = 0; argv[1][i]; i++) {
		if (argv[1][i] < '0' || argv[1][i] > '7') {
			puts("File mode must be an octal number");
			return -1;
		}
	}

	mode_t mode = strtol(argv[1], NULL, 8);

	int error = chmod(argv[2], mode);
	if (error < 0) {
		printf("Error while renaming %s: %d\n", argv[1], error);
		return error;
	}

	return 0;
}
