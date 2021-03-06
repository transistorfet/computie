
#ifndef _SRC_KERNEL_NET_PROTOCOL_IPV4_H
#define _SRC_KERNEL_NET_PROTOCOL_IPV4_H

#include <stdint.h>
#include "inet_af.h"

extern struct protocol ipv4_protocol;

struct ipv4_custom_data {
	struct ipv4_address src;
	struct ipv4_address dest;
};

uint16_t ipv4_calculate_checksum(void *data, int len, uint32_t start);

int ipv4_encode_header(struct packet *pack, const struct ipv4_address *src, const struct ipv4_address *dest, const unsigned char *data, int length);

#endif

