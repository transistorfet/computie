
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
	char state;
	struct dirent dir;
	char cmd[PS_BUFFER_SIZE];
	char buffer[PS_BUFFER_SIZE];
	int parent, pgid, session, ctty;

	if ((dd = open("/proc", 0, 0)) < 0) {
		printf("Error opening /proc\n");
		return dd;
	}

	printf("PID\tCMD\tSTATE\tPARENT\tPGID\n");
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
			snprintf(buffer, PS_BUFFER_SIZE, "/proc/%d/stat", pid);
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

			sscanf(buffer, "%*d %s %c %d %d %d %d\n", cmd, &state, &parent, &pgid, &session, &ctty);
			printf("%s\t%s\t%c\t%d\t%d\n", dir.d_name, cmd, state, parent, pgid);
		}
	}

	close(dd);

	return 0;
}
