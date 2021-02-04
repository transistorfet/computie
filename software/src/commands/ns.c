
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <asm/macros.h>

#include "prototype.h"

#define MAX_INPUT		20

int MAIN(command_ns)(int argc, char **argv)
{
	int i;
	int error;
	int nbytes;
	int sockfd;
	int sa_len;
	char buffer[MAX_INPUT];
	struct sockaddr_in addr;
	char *address = "192.168.1.102";

	if (argc >= 2)
		address = argv[1];

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		printf("Unable to create socket: %d\n", sockfd);
		return -1;
	}

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = to_be16(3904);
	inet_aton(address, &addr.sin_addr);
	//addr.sin_addr.s_addr = 0xC0A80166;

	while (1) {
		nbytes = read(STDIN_FILENO, buffer, MAX_INPUT);
		if (error < 0) {
			printf("Error reading: %d\n", error);
			break;
		}

		error = sendto(sockfd, buffer, nbytes, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
		if (error < 0) {
			printf("Error reading: %d\n", error);
			break;
		}
	}

	close(sockfd);

	return 0;
}

