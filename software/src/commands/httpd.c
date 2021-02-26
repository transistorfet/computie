
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
#define MAX_HEADERS		512


struct connection {
	int fd;
	int read;
	char headers[MAX_HEADERS];
};

struct connection *clients[MAX_CONNECTIONS];

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

struct connection *create_connection(int sockfd)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (!clients[i]) {
			clients[i] = malloc(sizeof(struct connection));
			clients[i]->fd = sockfd;
			clients[i]->read = 0;
			return clients[i];
		}
	}
	return NULL;
}

void close_connection(struct connection *conn)
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (clients[i] == conn) {
			printf("closing client %d\n", clients[i]->fd);
			close(clients[i]->fd);
			free(clients[i]);
			clients[i] = NULL;
			return;
		}
	}
}

int handle_connection(int listenfd)
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
	create_connection(sockfd);

	return 0;
}

int handle_read(struct connection *conn)
{
	int error;

	error = recv(conn->fd, &conn->headers[conn->read], MAX_HEADERS - conn->read - 1, 0);
	if (error == 0) {
		printf("client closed connection\n");
		return 1;
	}
	else if (error < 0) {
		printf("Error receiving: %d\n", error);
		return -1;
	}
	conn->read += error;
	printf("%d\n", conn->read);
	conn->headers[++conn->read] = '\0';

	printf("recv: %s\n", conn->headers);

	char *end = strstr(conn->headers, "\r\n\r\n");
	if (!end)
		return 0;

	printf("end of headers\n");

	int i = 0;
	char response[MAX_HEADERS];

	char *data = "This is a secret message\n";

	i += snprintf(&response[i], MAX_HEADERS - i, "HTTP/1.1 200 OK\r\n");
	i += snprintf(&response[i], MAX_HEADERS - i, "Content-Type: text/html; charset=UTF-8\r\n");
	i += snprintf(&response[i], MAX_HEADERS - i, "Content-Length: %d\r\n", strlen(data));
	i += snprintf(&response[i], MAX_HEADERS - i, "\r\n");
	i += snprintf(&response[i], MAX_HEADERS - i, data);

	printf("sending: %d, %s\n", i, response);

	error = send(conn->fd, response, i, 0);
	if (error < 0)
		printf("Error sending: %d\n", error);

	return 0;
}

int read_loop(int listenfd)
{
	int max;
	int error;
	fd_set rd;
	struct timeval timeout;

	while (1) {
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&rd);

		max = listenfd;
		FD_SET(listenfd, &rd);
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (clients[i]) {
				if (clients[i]->fd > max)
					max = clients[i]->fd;
				FD_SET(clients[i]->fd, &rd);
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
			handle_connection(listenfd);
		}

		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (clients[i] > 0 && FD_ISSET(clients[i]->fd, &rd)) {
				if (handle_read(clients[i]))
					close_connection(clients[i]);
			}
		}
	}
}

int run_server()
{
	int listenfd;

	for (int i = 0; i < MAX_CONNECTIONS; i++)
		clients[i] = NULL;

	listenfd = create_listener();
	if (listenfd < 0)
		return -1;

	read_loop(listenfd);

	close(listenfd);

	return 0;
}

