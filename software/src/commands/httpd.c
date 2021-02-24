
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

//#include <arpa/inet.h>
#include <asm/macros.h>
#define htobe16	to_be16

#define HTTP_PORT		8099
#define MAX_CONNECTIONS		20
#define MAX_INPUT		256

int run_server();

int main(int argc, char **argv)
{
	int pid;

	/*
	pid = fork();
	if (pid < 0)
		return pid;
	if (pid > 0)
		return 0;
	*/

	run_server();
	return 0;
}

int create_listener()
{
	int error;
	int listenfd;
	struct sigaction act;
	struct sockaddr_in addr;

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
	addr.sin_port = htobe16(HTTP_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	error = bind(listenfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	if (error) {
		printf("Error binding: %d\n", error);
		return -1;
	}

	error = listen(listenfd, 1);
	if (error) {
		printf("Error listening: %d\n", error);
		return -1;
	}

	return listenfd;
}

int handle_connection(int listenfd, int *clients)
{
	int sockfd;
	int sa_len;
	struct sockaddr_in addr;

	sa_len = sizeof(struct sockaddr_in);
	sockfd = accept(listenfd, (struct sockaddr *) &addr, &sa_len);
	if (sockfd < 0) {
		printf("Error accepting: %d\n", sockfd);
		return -1;
	}

	printf("Connection from %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (clients[i] < 0) {
			clients[i] = sockfd;
			break;
		}
	}

	return sockfd;
}

int handle_read(int sockfd)
{
	int error;
	char buffer[MAX_INPUT];

	error = recv(sockfd, buffer, MAX_INPUT, 0);
	if (error == 0) {
		printf("client closed connection\n");
		return 1;
	}
	else if (error < 0) {
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
	else if (!strcmp(buffer, "exit\n")) {
		return 1;
	}

	return 0;
}

int run_server()
{
	int max;
	int error;
	fd_set rd;
	int listenfd;
	struct timeval timeout;
	int clients[MAX_CONNECTIONS];

	for (int i = 0; i < MAX_CONNECTIONS; i++)
		clients[i] = -1;

	listenfd = create_listener();

	while (1) {
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&rd);

		max = listenfd;
		FD_SET(listenfd, &rd);
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (clients[i] > 0) {
				if (clients[i] > max)
					max = clients[i];
				FD_SET(clients[i], &rd);
			}
		}

		error = select(max + 1, &rd, NULL, NULL, &timeout);
		if (error < 0) {
			if (error == EINTR)
				continue;
			printf("Error during select: %d\n", error);
			return -1;
		}

		if (error == 0)
			continue;

		if (FD_ISSET(listenfd, &rd)) {
			handle_connection(listenfd, clients);
		}

		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (clients[i] > 0 && FD_ISSET(clients[i], &rd)) {
				if (handle_read(clients[i])) {
					printf("closing client %d\n", clients[i]);
					close(clients[i]);
					clients[i] = -1;
				}
			}
		}
	}

	close(listenfd);

	return 0;
}

