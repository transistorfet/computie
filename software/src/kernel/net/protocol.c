
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

int net_register_protocol(struct protocol *proto)
{
	for (short i = 0; i < PROTOCOLS_MAX; i++) {
		if (!proto_list[i]) {
			proto_list[i] = proto;
			return 0;
		}
	}
	return -1;
}

struct protocol *net_get_protocol(int domain, int type, int protocol)
{
	for (short i = 0; i < PROTOCOLS_MAX; i++) {
		if (proto_list[i] && proto_list[i]->domain == domain && (!protocol || proto_list[i]->protocol == protocol) && (!type || proto_list[i]->type == type))
			return proto_list[i];
	}
	return NULL;
}

struct packet *net_create_packet(struct protocol *proto, const struct address *src, const struct address *dest, const unsigned char *buf, uint16_t nbytes)
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

int net_incoming_packet(struct protocol *proto, struct packet *pack)
{
	int error;

	pack->domain = proto->domain;
	error = proto->ops->decode_header(proto, pack, 0);
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


int net_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result)
{
	return proto->ops->create_endpoint(proto, sock, sockaddr, len, result);
}

int net_destroy_endpoint(struct endpoint *ep)
{
	return ep->ops->destroy(ep);
}

int net_connect_endpoint(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len)
{
	return ep->ops->connect(ep, sockaddr, len);
}

int net_endpoint_send_to(struct endpoint *ep, const char *buf, int nbytes, const struct sockaddr *sockaddr, socklen_t len)
{
	return ep->ops->send_to(ep, buf, nbytes, sockaddr, len);
}

int net_endpoint_recv_from(struct endpoint *ep, char *buf, int nbytes, struct sockaddr *sockaddr, socklen_t *len)
{
	return ep->ops->recv_from(ep, buf, nbytes, sockaddr, len);
}



