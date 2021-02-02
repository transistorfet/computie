
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

// TODO this should be removed because it's domain-specific
#include <netinet/in.h>

#include "socket.h"
#include "protocol.h"

#include "../misc/queue.h"

#define PROTOCOLS_MAX		4


static struct protocol *proto_list[PROTOCOLS_MAX];


int init_net_protocol()
{
	for (short i = 0; i < PROTOCOLS_MAX; i++)
		proto_list[i] = NULL;
}

int register_protocol(struct protocol *proto)
{
	for (short i = 0; i < PROTOCOLS_MAX; i++) {
		if (!proto_list[i]) {
			proto_list[i] = proto;
			return 0;
		}
	}
	return -1;
}

struct protocol *get_protocol(int domain, int type, int protocol)
{
	for (short i = 0; i < PROTOCOLS_MAX; i++) {
		if (proto_list[i] && proto_list[i]->domain == domain && (!protocol || proto_list[i]->protocol == protocol) && (!type || proto_list[i]->type == type))
			return proto_list[i];
	}
	return NULL;
}

int add_protocol_listener(struct protocol *proto, struct socket *sock)
{
	struct sockaddr *addr;

	// TODO this needs to be protocol agnostic somehow, possibly by making protocol-specific implementations
	for (struct queue_node *cur = proto->connected_sockets.head; cur; cur = cur->next) {
		addr = (struct sockaddr *) &((struct socket *) cur)->local;
		if (((struct sockaddr_in *) addr)->sin_port == ((struct sockaddr_in *) &sock->local)->sin_port)
		// TODO the 16 here for the sockaddr len is a hack to remember to fix this later
		//if (!memcmp(addr, &sock->local, 16))
			return EADDRINUSE;
	}

	_queue_insert_after(&proto->connected_sockets, &sock->node, proto->connected_sockets.tail);
	return 0;
}

int remove_protocol_listener(struct protocol *proto, struct socket *sock)
{
	for (struct queue_node *cur = proto->connected_sockets.head; cur; cur = cur->next) {
		if (cur == &sock->node) {
			_queue_remove(&proto->connected_sockets, &sock->node);
			return 0;
		}
	}
	return -1;
} 

struct socket *find_protocol_listener(struct protocol *proto, struct sockaddr *addr, socklen_t len)
{
	struct sockaddr *sockaddr;

	for (struct queue_node *cur = proto->connected_sockets.head; cur; cur = cur->next) {
		sockaddr = (struct sockaddr *) &((struct socket *) cur)->local;
		if (((struct sockaddr_in *) addr)->sin_port == ((struct sockaddr_in *) sockaddr)->sin_port)
		//if (!memcmp(sockaddr, addr, len))
			return (struct socket *) cur;
	}
	return NULL;
}

struct packet *create_protocol_packet(struct protocol *proto, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *buf, uint16_t nbytes)
{
	int error;
	struct packet *pack;

	// TODO maybe you should instead pass in a buffer for the packet header, and then assemble the packet here?  instead of allocating more memory than needed
	pack = packet_alloc(NULL, nbytes + 100);
	pack->domain = proto->domain;
	pack->protocol = proto->protocol;

	error = proto->ops->encode_header(proto, pack, src, dest, buf, nbytes);
	if (error) {
		packet_free(pack);
		return NULL;
	}

	pack->data_offset = pack->length;
	error = packet_append(pack, buf, nbytes);
	if (error) {
		packet_free(pack);
		return NULL;
	}

	return pack;
}

int parse_protocol_packet(struct protocol *proto, struct packet *pack)
{
	int error;
	int offset = 0;

	pack->domain = proto->domain;
	error = proto->ops->decode_header(proto, pack, offset);
	if (error)
		return error;

	return 0;
}

int forward_protocol_packet(struct protocol *proto, struct packet *pack)
{
	int error;

	error = parse_protocol_packet(proto, pack);
	if (error) {
		packet_free(pack);
		return PACKET_ERROR;
	}

	error = proto->ops->forward_packet(proto, pack);
	if (error == PACKET_DELIVERED)
		return PACKET_DELIVERED;
	
	packet_free(pack);
	return PACKET_DROPPED;
}

