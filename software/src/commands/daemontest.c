
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int fd;
	int pid;
	char ch;
	int error;

	pid = fork();
	if (pid) {
		printf("Detaching\n");
		return 0;
	}
	else {
		fd = open("/dev/tty1", 0, 0);
		if (fd < 0) {
			printf("Unable to open device: %d\n", fd);
			return -1;
		}

		while (1) {
			error = read(fd, &ch, 1);

			if (error < 0) {
				printf("Unable to read device: %d\n", error);
				return -1;
			}

			putchar(ch);
		}

		printf("Closing and exiting\n");
		close(fd);
	}

	return 0;
}


