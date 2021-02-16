
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <asm/macros.h>

#define MAX_INPUT		20

int main(int argc, char **argv)
{
	int i;
	int error;
	int nbytes;
	int sa_len;
	int sockfd;
	int listenfd;
	struct sigaction act;
	char buffer[MAX_INPUT];
	struct sockaddr_in addr;

	int port = 3904;

	if (argc >= 2)
		port = strtol(argv[1], NULL, 10);

	act.sa_handler = NULL;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);

	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		printf("Unable to create socket: %d\n", listenfd);
		return -1;
	}

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = to_be16(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	error = bind(listenfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	if (error) {
		printf("Error while binding: %d\n", error);
		return -1;
	}

	error = listen(listenfd, 1);
	if (error) {
		printf("Error while listening: %d\n", error);
		return -1;
	}

	while (1) {
		sa_len = sizeof(struct sockaddr_in);
		sockfd = accept(listenfd, (struct sockaddr *) &addr, &sa_len);
		if (sockfd < 0) {
			printf("Error accepting: %d\n", sockfd);
			break;
		}

		printf("Connection from %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);
		while (1) {
			error = recv(sockfd, buffer, MAX_INPUT, 0);
			if (error <= 0) {
				printf("Error receiving: %d\n", error);
				return -1;
			}
			buffer[error] = '\0';

			printf("recv: %s\n", buffer);

			if (!strcmp(buffer, "test\n")) {
				error = send(sockfd, "I heard you\n", 12, 0);
				if (error < 0) {
					printf("Error sending: %d\n", error);
					return -1;
				}
			}
			else if (!strcmp(buffer, "light_on\n")) {
				int data = 0x01;
				if (!ioctl(STDIN_FILENO, TSETLEDS, &data)) {
					send(sockfd, "ok\n", 3, 0);
				}
				else {
					send(sockfd, "error\n", 6, 0);
				}
			}
			else if (!strcmp(buffer, "light_off\n")) {
				int data = 0x00;
				if (!ioctl(STDIN_FILENO, TSETLEDS, &data)) {
					send(sockfd, "ok\n", 3, 0);
				}
				else {
					send(sockfd, "error\n", 6, 0);
				}
			}
			else if (!strcmp(buffer, "exit\n")) {
				break;
			}
		}

		close(sockfd);
	}

	close(listenfd);

	return 0;
}

