
#ifndef _SRC_KERNEL_NET_PROTOCOL_H
#define _SRC_KERNEL_NET_PROTOCOL_H

#include <stdint.h>
#include <sys/socket.h>

#include "packet.h"

#include "../misc/queue.h"

#define PACKET_DELIVERED	1
#define PACKET_DROPPED		0
#define PACKET_ERROR		-1

#define SAT_SRC			1
#define SAT_DEST		2

struct socket;
struct protocol;

struct protocol_ops {
	int (*init)();
	int (*encode_header)(struct protocol *proto, struct packet *pack, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *data, int length);
	int (*decode_header)(struct protocol *proto, struct packet *pack, uint16_t offset);

	int (*forward_packet)(struct protocol *proto, struct packet *pack);
	int (*fetch_sockaddr)(struct protocol *proto, struct packet *pack, int type, struct sockaddr *sockaddr, socklen_t len);
};

struct protocol {
	struct protocol_ops *ops;
	struct protocol *next;
	uint8_t domain;
	uint8_t type;
	uint8_t protocol;

	struct queue connected_sockets;
};

int init_net_protocol();
int register_protocol(struct protocol *proto);
struct protocol *get_protocol(int domain, int type, int protocol);

int add_protocol_listener(struct protocol *proto, struct socket *sock);
int remove_protocol_listener(struct protocol *proto, struct socket *sock);
struct socket *find_protocol_listener(struct protocol *proto, struct sockaddr *addr, socklen_t len);

struct packet *create_protocol_packet(struct protocol *proto, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *buf, uint16_t nbytes);
int parse_protocol_packet(struct protocol *proto, struct packet *pack);
int forward_protocol_packet(struct protocol *proto, struct packet *pack);

#endif

