
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

#define CP_BUF_SIZE	512

int MAIN(command_cp)(int argc, char **argv, char **envp)
{
	int result;
	int src_fd, dest_fd;
	char buffer[CP_BUF_SIZE];

	if (argc <= 2) {
		puts("Usage: cp <source> <dest>");
		return -1;
	}


	if (!(src_fd = open(argv[1], O_RDONLY, 0))) {
		printf("Error when opening %s: %d\n", argv[1], src_fd);
		return -1;
	}

	if (!(dest_fd = open(argv[2], O_WRONLY | O_CREAT, 0644))) {
		printf("Error when opening %s: %d\n", argv[2], dest_fd);
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
