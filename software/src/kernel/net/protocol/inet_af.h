
#ifndef _SRC_KERNEL_NET_PROTOCOL_INET_AF_H
#define _SRC_KERNEL_NET_PROTOCOL_INET_AF_H

#include <stdint.h>
#include <sys/socket.h>

struct ipv4_address {
	uint32_t addr;
	uint16_t port;
};


int inet_load_sockaddr(struct sockaddr *sa, socklen_t *sa_len, struct ipv4_address *addr);
int inet_resolve_address(const struct sockaddr *requested, socklen_t len, const struct sockaddr *actual, struct ipv4_address *ipv4);

#endif

