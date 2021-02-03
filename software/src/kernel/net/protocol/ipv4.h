
#ifndef _SRC_KERNEL_NET_PROTOCOL_IPV4_H
#define _SRC_KERNEL_NET_PROTOCOL_IPV4_H

#include <stdint.h>

extern struct protocol ipv4_protocol;

struct ipv4_address {
	uint32_t addr;
};

struct ipv4_custom_data {
	struct ipv4_address src;
	struct ipv4_address dest;
};

uint16_t ipv4_calculate_checksum(void *data, int len);

#endif

