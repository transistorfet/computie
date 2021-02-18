
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <asm/macros.h>

#include "prototype.h"

#define MAX_INPUT		20

int client_loop(int sockfd, int f_udp, char *address, int port);
int listen_loop(int sockfd, int f_udp, int port);

int MAIN(command_ns)(int argc, char **argv)
{
	int opt;
	int sockfd;

	const char *usage = "Usage: ns [-ul] [<address> [<port>]]";
	char *address = "192.168.1.102";
	int port = 3904;
	int f_listen = 0, f_udp = 0;

	// TODO this is only needed if compiled into shell, so remove it
	optind = 1;

	while ((opt = getopt(argc, argv, "ul")) != EOF) {
		switch (opt) {
		    case '?':
			puts(usage);
			return -1;
		    case 'u':
			f_udp = 1;
			break;
		    case 'l':
			f_listen = 1;
			break;
		    default:
			// TODO this is for debugging
			printf(">> %c\n", opt);
			break;
		}
	}

	if (argc - optind > 2 - f_listen) {
		puts(usage);
		return -1;
	}

	//printf("%s %s:%d using %s\n", f_listen ? "listening on" : "connecting to", address, port, f_udp ? "UDP" : "TCP");

	if (!f_listen && optind < argc)
		address = argv[optind++];
	if (optind < argc)
		port = strtol(argv[optind++], NULL, 10);


	sockfd = socket(PF_INET, f_udp ? SOCK_DGRAM : SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Unable to create socket: %d\n", sockfd);
		return -1;
	}

	if (f_listen)
		listen_loop(sockfd, f_udp, port);
	else
		client_loop(sockfd, f_udp, address, port);

	close(sockfd);

	return 0;
}

int client_loop(int sockfd, int f_udp, char *address, int port)
{
	int error;
	int nbytes;
	char buffer[MAX_INPUT];
	struct sockaddr_in addr;

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = to_be16(port);
	inet_aton(address, &addr.sin_addr);

	if (!f_udp) {
		error = connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
		if (error) {
			printf("Error connecting: %d\n", error);
			return -1;
		}
	}

	while (1) {
		nbytes = read(STDIN_FILENO, buffer, MAX_INPUT);
		if (nbytes < 0) {
			printf("Error reading: %d\n", nbytes);
			break;
		}

		if (f_udp)
			error = sendto(sockfd, buffer, nbytes, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
		else
			error = send(sockfd, buffer, nbytes, 0);

		if (error < 0) {
			printf("Error sending: %d\n", error);
			break;
		}
	}

	return 0;
}

int listen_loop(int sockfd, int f_udp, int port)
{
	int nbytes;
	int error;
	int sa_len;
	char buffer[MAX_INPUT];
	struct sockaddr_in addr;

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = to_be16(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	error = bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	if (error) {
		printf("Error binding: %d\n", error);
		return -1;
	}

	if (!f_udp) {
		error = listen(sockfd, 1);
		if (error) {
			printf("Error listening: %d\n", error);
			return -1;
		}

		sa_len = sizeof(struct sockaddr_in);
		sockfd = accept(sockfd, (struct sockaddr *) &addr, &sa_len);
		if (sockfd < 0) {
			printf("Error accepting: %d\n", sockfd);
			return -1;
		}

		printf("Connection from %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);
	}

	while (1) {
		sa_len = sizeof(struct sockaddr_in);
		if (f_udp)
			nbytes = recvfrom(sockfd, buffer, MAX_INPUT, 0, (struct sockaddr *) &addr, &sa_len);
		else
			nbytes = recv(sockfd, buffer, MAX_INPUT, 0);

		if (nbytes <= 0) {
			printf("Error receiving: %d\n", nbytes);
			return -1;
		}
		buffer[nbytes] = '\0';

		if (f_udp)
			printf("%s:%d >> %d: %s", inet_ntoa(addr.sin_addr), addr.sin_port, nbytes, buffer);
		else
			printf("%d: %s", nbytes, buffer);
	}

	close(sockfd);

	return 0;
}

