
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_kill)(int argc, char **argv, char **envp)
{
	int error;
	int i = 1;
	pid_t pid;
	int signal = 6;
	char *endptr;

	if (argc <= 1) {
		printf("Usage: kill -[\\d] <pid>\n");
		return 0;
	}

	if (argv[i][0] == '-') {
		signal = strtol(&argv[i][1], &endptr, 10);
		i += 1;
	}

	pid = strtol(argv[i], &endptr, 10);
	if (pid < 0 || endptr[0] != '\0') {
		printf("Invalid pid number\n");
		return -1;
	}

	error = kill(pid, signal);
	if (error < 0) {
		printf("Error attempting to send signal %d to process %d, return %d\n", signal, pid, error);
		return -1;
	}

	return 0;
}
