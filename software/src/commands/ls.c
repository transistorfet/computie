
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

void format_file_mode(mode_t mode, char *buffer)
{
	mode_t curbit = 0400;

	strcpy(buffer, "-rwxrwxrwx");

	if (mode & S_IFDIR)
		buffer[0] = 'd';

	for (char i = 1; i <= 10; i++) {
		if (!(mode & curbit))
			buffer[i] = '-';
		curbit >>= 1;
	}
}

int MAIN(command_ls)(int argc, char **argv, char **envp)
{
	int fd;
	int error;
	struct dirent dir;
	struct stat statbuf;
	char filename[100];
	char filemode[10];

	char *path = argc > 1 ? argv[1] : ".";

	if ((fd = open(path, 0, 0)) < 0) {
		printf("Error opening %s: %d\n", path, fd);
		return fd;
	}

	int start = strlen(path) - 1;
	strcpy(filename, path);
	if (filename[start] != '/')
		filename[++start] = '/';
	start++;

	while (1) {
		error = readdir(fd, &dir);
		if (error < 0) {
			printf("Error at readdir %d\n", error);
			return error;
		}

		if (error == 0)
			break;

		if (dir.d_name[0] != '.') {
			strcpy(&filename[start], dir.d_name);
			error = stat(filename, &statbuf);
			if (error < 0) {
				printf("Error at stat %s (%d)\n", filename, error);
				return error;
			}

			format_file_mode(statbuf.st_mode, filemode);
			printf("%s %6d %s\n", filemode, statbuf.st_size, dir.d_name);
		}
	}

	close(fd);

	return 0;
}

