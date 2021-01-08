
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "prototype.h"

#define CAT_BUF_SIZE	256

int MAIN(command_cat)(int argc, char **argv, char **envp)
{
	int fd;
	int result;
	char buffer[CAT_BUF_SIZE];

	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
		printf("Error opening %s: %d\n", argv[1], fd);
		return fd;
	}

	while (1) {
		result = read(fd, buffer, CAT_BUF_SIZE);
		if (result == 0)
			break;

		if (result < 0) {
			printf("Error while reading: %d\n", result);
			return result;
		}

		write(STDOUT_FILENO, buffer, result);
	}

	putchar('\n');

	close(fd);

	return 0;
}
