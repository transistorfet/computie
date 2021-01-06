
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT	100

int process_input(char *input);

int main()
{
	int i;
	int fd;
	int pid;
	int error;
	char buffer[MAX_INPUT];

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

		// Send a request over serial for the current time
		write(fd, "time\n", 5);

		i = 0;
		while (1) {
			if (i >= MAX_INPUT)
				i = 0;
			error = read(fd, &buffer[i], 1);

			if (error < 0) {
				printf("Unable to read device: %d\n", error);
				return -1;
			}

			//putchar(ch);
			if (buffer[i] == '\n') {
				buffer[i] = '\0';
				printf("%d: %s\n", i, buffer);
				process_input(buffer);
				i = 0;
			}
			else
				i++;
		}

		printf("Closing and exiting\n");
		close(fd);
	}

	return 0;
}

int process_input(char *input)
{
	// TODO process the input
	return 0;
}

