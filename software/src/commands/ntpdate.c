
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <asm/macros.h>

struct ntp_packet {
	//char data[48];
	uint8_t mode;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;

	uint32_t delay;
	uint32_t dispersion;
	uint32_t id;

	uint32_t ref_ts_s;
	uint32_t ref_ts_f;

	uint32_t origin_ts_s;
	uint32_t origin_ts_f;

	uint32_t receive_ts_s;
	uint32_t receive_ts_f;

	uint32_t transmit_ts_s;
	uint32_t transmit_ts_f;
};

int main(int argc, char **argv)
{
	int i;
	time_t t;
	int error;
	int nbytes;
	int sockfd;
	int sa_len;
	struct ntp_packet tx;
	struct ntp_packet rx;
	struct sockaddr_in addr;

	char *address = "51.38.105.7";	// pool.ntp.org
	int port = 123;

	if (argc >= 2)
		address = argv[1];
	if (argc >= 3)
		port = strtol(argv[2], NULL, 10);

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		printf("Unable to create socket: %d\n", sockfd);
		return -1;
	}

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = to_be16(port);
	inet_aton(address, &addr.sin_addr);
	//addr.sin_addr.s_addr = 0xC0A80166;

	memset(&tx, '\0', sizeof(struct ntp_packet));
	tx.mode = '\x1b';
	error = sendto(sockfd, &tx, sizeof(struct ntp_packet), 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
	if (error < 0) {
		printf("Error sending: %d\n", error);
		return -1;
	}

	nbytes = recvfrom(sockfd, &rx, sizeof(struct ntp_packet), 0, NULL, NULL);
	if (error < 0) {
		printf("Error reading: %d\n", error);
		return -1;
	}
	t = rx.transmit_ts_s  - 2208988800;
	printf("new timestamp: %d\n", t);
	stime(&t);

	close(sockfd);

	return 0;
}

