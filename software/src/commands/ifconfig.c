
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <asm/macros.h>

int main(int argc, char **argv)
{
	char up;
	int flags;
	int error;
	int sockfd;
	char *device;
	struct ifreq ifreq;
	char *address = NULL;

	up = 0;
	if (argc >= 3) {
		if (!strcmp(argv[argc - 1], "up"))
			up = 1;
		else if (!strcmp(argv[argc - 1], "down"))
			up = 2;
		else
			up = -1;
	}

	if (up == -1 || argc < 2 || argc > 4) {
		puts("Usage: ifconfig <iface> [<ip address> <up/down>]");
		return -1;
	}

	device = argv[1];
	if (argc == 4)
		address = argv[2];

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		printf("Unable to create socket: %d\n", sockfd);
		return -1;
	}

	if (up) {
		strcpy(ifreq.ifr_name, device);

		if (address) {
			memset(&ifreq.ifr_addr, '\0', sizeof(struct sockaddr_in));
			((struct sockaddr_in *) &ifreq.ifr_addr)->sin_family = AF_INET;
			((struct sockaddr_in *) &ifreq.ifr_addr)->sin_port = 0;
			inet_aton(address, &((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr);

			error = ioctl(sockfd, SIOCSIFADDR, &ifreq);
			if (error) {
				printf("Error setting address: %d\n", error);
				return -1;
			}
		}

		ifreq.ifr_flags = (up == 1 ? IFF_UP : 0) | IFF_DEBUG;
		error = ioctl(sockfd, SIOCSIFFLAGS, &ifreq);
		if (error) {
			printf("Error bringing device %s: %d\n", up == 1 ? "up" : "down", error);
			return -1;
		}
	}
	else {
		strcpy(ifreq.ifr_name, device);

		error = ioctl(sockfd, SIOCGIFFLAGS, &ifreq);
		if (error) {
			printf("Error setting address: %d\n", error);
			return -1;
		}
		flags = ifreq.ifr_flags;

		error = ioctl(sockfd, SIOCGIFADDR, &ifreq);
		if (error) {
			printf("Error setting address: %d\n", error);
			return -1;
		}

		if (ifreq.ifr_addr.ss_family != AF_INET) {
			printf("Unexpected address domain: %d\n", ifreq.ifr_addr.ss_family);
			return -1;
		}

		printf("%s: flags=%d<%s>\n", device, flags, (flags & IFF_UP) ? "UP" : "DOWN");
		printf("\tinet: %s\n", inet_ntoa(((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr));
	}

	close(sockfd);

	return 0;
}

