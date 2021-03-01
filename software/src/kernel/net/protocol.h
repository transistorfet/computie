
#ifndef _SRC_KERNEL_NET_PROTOCOL_H
#define _SRC_KERNEL_NET_PROTOCOL_H

#include <stdint.h>
#include <sys/socket.h>

#include "../misc/queue.h"

#include "if.h"
#include "packet.h"


#define PACKET_DELIVERED	1
#define PACKET_DROPPED		0
#define PACKET_ERROR		-1

struct socket;
struct protocol;
struct endpoint;

struct protocol_ops {
	int (*init)();
	//int (*transform_packet)(struct protocol *proto, struct packet *pack, int request, void *argp);
	int (*decode_header)(struct protocol *proto, struct packet *pack, uint16_t offset);
	int (*forward_packet)(struct protocol *proto, struct packet *pack);
	int (*create_endpoint)(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result);
};

struct protocol {
	struct protocol_ops *ops;
	struct protocol *next;
	uint8_t domain;
	uint8_t type;
	uint8_t protocol;
};

struct endpoint_ops {
	int (*destroy)(struct endpoint *ep);
	int (*listen)(struct endpoint *ep, int queue);
	int (*accept)(struct endpoint *ep, struct sockaddr *sockaddr, socklen_t *len, struct endpoint **result);
	int (*connect)(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len);
	int (*shutdown)(struct endpoint *ep, int how);
	int (*send)(struct endpoint *ep, const char *buf, int nbytes);
	int (*recv)(struct endpoint *ep, char *buf, int max);
	int (*send_to)(struct endpoint *ep, const char *buf, int nbytes, const struct sockaddr *sockaddr, socklen_t len);
	int (*recv_from)(struct endpoint *ep, char *buf, int max, struct sockaddr *sockaddr, socklen_t *len);
	int (*get_options)(struct endpoint *ep, int level, int optname, void *optval, socklen_t *optlen);
	int (*set_options)(struct endpoint *ep, int level, int optname, const void *optval, socklen_t optlen);
	int (*poll)(struct endpoint *ep, int events);
};

struct endpoint {
	struct queue_node node;
	struct endpoint_ops *ops;
	struct protocol *proto;
	struct socket *sock;
	struct if_device *ifdev;
};


int init_net_protocol();
int net_register_protocol(struct protocol *proto);
struct protocol *net_get_protocol(int domain, int type, int protocol);

int net_incoming_packet(struct protocol *proto, struct packet *pack);

int net_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result);
int net_destroy_endpoint(struct endpoint *ep);

#endif

