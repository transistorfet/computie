
#ifndef _SRC_KERNEL_NET_PACKET_H
#define _SRC_KERNEL_NET_PACKET_H

#include <stddef.h>
#include "../misc/queue.h"

#define PACKET_CUSTOM_DATA	16

struct socket;
struct if_device;

struct packet {
	struct queue_node node;
	struct socket *sock;
	struct if_device *ifdev;

	uint8_t domain;
	uint8_t protocol;

	uint16_t network_offset;
	uint16_t transport_offset;
	uint16_t data_offset;

	//char *custom_data[PACKET_CUSTOM_DATA];

	uint16_t length;
	uint16_t capacity;
	unsigned char data[0];
};

struct packet *packet_alloc(struct if_device *dev, size_t capacity);
void packet_free(struct packet *pack);
int packet_append(struct packet *pack, const void *buf, int nbytes);

#endif
