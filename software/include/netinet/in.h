
#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <sys/socket.h>

#define IPPROTO_IP		0
#define IPPROTO_ICMP		1
#define IPPROTO_IGMP		2
#define IPPROTO_TCP		6
#define IPPROTO_UDP		17


#define	INADDR_ANY		((in_addr_t) 0x00000000)
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)
#define	INADDR_NONE		((in_addr_t) 0xffffffff)

typedef uint16_t in_port_t;

typedef uint32_t in_addr_t;

struct in_addr {
	in_addr_t s_addr;
};

struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero[8];
};

int inet_aton(const char *str, struct in_addr *ptr);
char *inet_ntoa(struct in_addr in);

#endif

