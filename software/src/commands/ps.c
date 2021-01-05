
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "prototype.h"

#define PS_BUFFER_SIZE	100

int MAIN(command_ps)(int argc, char **argv, char **envp)
{
	pid_t pid;
	int error;
	int dd, fd;
	struct dirent dir;
	char buffer[PS_BUFFER_SIZE];

	if ((dd = open("/proc", 0, 0)) < 0) {
		printf("Error opening /proc\n");
		return dd;
	}

	printf("PID\tCMD\n");
	while (1) {
		error = readdir(dd, &dir);
		if (error < 0) {
			printf("Error at readdir %d\n", error);
			return error;
		}

		if (error == 0)
			break;

		pid = strtol(dir.d_name, NULL, 10);
		if (pid > 0) {
			snprintf(buffer, PS_BUFFER_SIZE, "/proc/%d/cmdline", pid);
			if ((fd = open(buffer, 0, 0)) < 0) {
				printf("Error opening %s\n", buffer);
				return fd;
			}

			error = read(fd, buffer, PS_BUFFER_SIZE);
			if (error < 0) {
				printf("Error reading %s (%d)\n", buffer, error);
				return error;
			}

			buffer[error] = '\0';

			close(fd);

			printf("%s\t%s\n", dir.d_name, buffer);
		}
	}

	close(dd);

	return 0;
}
