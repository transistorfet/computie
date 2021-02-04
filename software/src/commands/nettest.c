
#include <asm/macros.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "prototype.h"

#define MAX_INPUT	100

int serverloop();
int process_input(char *input);

int MAIN(command_nettest)()
{
	int pid;

	pid = fork();
	if (pid) {
		printf("Detaching\n\0");
		return 0;
	}
	else {
		serverloop();
	}

	return 0;
}

int serverloop()
{
	int i;
	int error;
	int sockfd;
	int sa_len;
	char buffer[MAX_INPUT];
	struct sockaddr_in addr;

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		printf("Unable to create socket: %d\n", sockfd);
		return -1;
	}

	printf("Created socket %d\n", sockfd);

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = to_be16(3904);
	addr.sin_addr.s_addr = INADDR_ANY;

	error = bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	if (error) {
		printf("Error binding socket %d\n", error);
		return -1;
	}

	//inet_aton("63.161.169.137", &myaddr.sin_addr.s_addr);
	addr.sin_addr.s_addr = 0xC0A80166;

	strcpy(buffer, "Test\n");
	sendto(sockfd, buffer, 5, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	//send(sockfd, buffer, 5, 0);
	//connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));

	while (1) {
		sa_len = sizeof(struct sockaddr_in);
		i = recvfrom(sockfd, buffer, MAX_INPUT, 0, (struct sockaddr *) &addr, &sa_len);
		buffer[i] = '\0';
		printf("%s:%d >> %d: %s\n", inet_ntoa(addr.sin_addr), addr.sin_port, i, buffer);
	}

	close(sockfd);

	/*
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
	*/
}

int process_input(char *input)
{
	// TODO process the input
	return 0;
}

