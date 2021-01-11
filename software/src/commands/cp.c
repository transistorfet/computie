
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "prototype.h"

#define CP_BUF_SIZE	512

static const char *cp_path_last_component(const char *path);

int MAIN(command_cp)(int argc, char **argv, char **envp)
{
	int result;
	struct stat statbuf;
	int src_fd, dest_fd;
	char *srcfile, *destfile;
	char buffer[CP_BUF_SIZE];

	if (argc <= 2) {
		puts("Usage: cp <source> <dest>");
		return -1;
	}

	srcfile = argv[1];
	destfile = argv[2];

	result = stat(destfile, &statbuf);
	if (!result && statbuf.st_mode & S_IFDIR) {
		int end = strlen(destfile) - 1;
		strncpy(buffer, destfile, CP_BUF_SIZE);
		if (buffer[end] != '/')
			buffer[++end] = '/';
		strncpy(&buffer[end + 1], cp_path_last_component(srcfile), CP_BUF_SIZE - (end + 1));
		destfile = buffer;
	}

	if (!(src_fd = open(srcfile, O_RDONLY, 0))) {
		printf("Error when opening %s: %d\n", srcfile, src_fd);
		return -1;
	}

	if (!(dest_fd = open(destfile, O_WRONLY | O_CREAT, 0644))) {
		printf("Error when opening %s: %d\n", destfile, dest_fd);
		close(src_fd);
		return -1;
	}

	while (1) {
		result = read(src_fd, buffer, CP_BUF_SIZE);
		if (result == 0)
			break;

		if (result < 0) {
			printf("Error while reading: %d\n", result);
			return result;
		}

		result = write(dest_fd, buffer, result);

		if (result < 0) {
			printf("Error while writing: %d\n", result);
			return result;
		}
	}

	close(dest_fd);
	close(src_fd);

	return 0;
}

static const char *cp_path_last_component(const char *path)
{
	short i = strlen(path) - 1;
	if (path[i] == '/')
		i--;
	for (; i >= 0; i--) {
		if (path[i] == '/')
			break;
	}
	i += 1;
	return &path[i];
}

