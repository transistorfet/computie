
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "prototype.h"

#define MV_BUF_SIZE	256

static const char *mv_path_last_component(const char *path);

int MAIN(command_mv)(int argc, char **argv, char **envp)
{
	int error;
	struct stat statbuf;
	char *srcfile, *destfile;
	char buffer[MV_BUF_SIZE];

	if (argc <= 2) {
		puts("Usage: mv <file> <dest>");
		return -1;
	}

	srcfile = argv[1];
	destfile = argv[2];

	error = stat(destfile, &statbuf);
	if (!error && statbuf.st_mode & S_IFDIR) {
		int end = strlen(destfile) - 1;
		strncpy(buffer, destfile, MV_BUF_SIZE);
		if (buffer[end] != '/')
			buffer[++end] = '/';
		strncpy(&buffer[end + 1], mv_path_last_component(srcfile), MV_BUF_SIZE - (end + 1));
		destfile = buffer;
	}

	error = rename(srcfile, destfile);
	if (error < 0) {
		printf("Error while renaming %s: %d\n", srcfile, error);
		return error;
	}

	return 0;
}

static const char *mv_path_last_component(const char *path)
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

