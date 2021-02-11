
#include <errno.h>
#include <string.h>
#include <asm/macros.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <kernel/kmalloc.h>

#include "../if.h"
#include "../socket.h"
#include "../protocol.h"

#include "ipv4.h"
#include "inet_af.h"

#define UDP_ADDRESS(x)		((struct udp_address *) (x))
#define UDP_ENDPOINT(x)		((struct udp_endpoint *) (x))


int udp_init();
int udp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int udp_forward_packet(struct protocol *proto, struct packet *pack);
int udp_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result);

int udp_connect_endpoint(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len);
int udp_destroy_endpoint(struct endpoint *ep);
int udp_endpoint_send_to(struct endpoint *ep, const char *buf, int nbytes, const struct sockaddr *sockaddr, socklen_t len);
int udp_endpoint_recv_from(struct endpoint *ep, char *buf, int nbytes, struct sockaddr *sockaddr, socklen_t *len);
int udp_endpoint_get_options(struct endpoint *ep, int level, int optname, void *optval, socklen_t *optlen);
int udp_endpoint_set_options(struct endpoint *ep, int level, int optname, const void *optval, socklen_t optlen);


struct protocol_ops udp_protocol_ops = {
	udp_init,
	udp_decode_header,
	udp_forward_packet,
	udp_create_endpoint,
};

struct protocol udp_protocol = {
	&udp_protocol_ops,
	&ipv4_protocol,
	PF_INET,
	SOCK_DGRAM,
	IPPROTO_UDP,
	{ NULL },
};

struct endpoint_ops udp_endpoint_ops = {
	udp_connect_endpoint,
	udp_destroy_endpoint,
	NULL,
	NULL,
	udp_endpoint_send_to,
	udp_endpoint_recv_from,
	udp_endpoint_get_options,
	udp_endpoint_set_options,
};

struct udp_endpoint {
	struct endpoint ep;
	struct ipv4_address src;
	struct ipv4_address dest;
};


struct udp_header {
	uint16_t src;
	uint16_t dest;
	uint16_t length;
	uint16_t checksum;
};

static struct endpoint *lookup_endpoint(struct protocol *proto, uint32_t addr, uint16_t port);


int udp_init()
{
	net_register_protocol(&udp_protocol);
}

int udp_encode_packet(struct packet *pack, const struct ipv4_address *src, const struct ipv4_address *dest, const unsigned char *data, int length)
{
	int error;
	struct udp_header hdr;

	error = ipv4_encode_header(pack, src, dest, data, sizeof(struct udp_header) + length);
	if (error)
		return error;

	hdr.src = to_be16(src->port);
	hdr.dest = to_be16(dest->port);
	hdr.length = to_be16(sizeof(struct udp_header) + length);
	// TODO calculate checksum
	hdr.checksum = to_be16(0);

	error = packet_append(pack, &hdr, sizeof(struct udp_header));
	if (error)
		return error;

	pack->data_offset = pack->length;
	if (packet_append(pack, data, length)) {
		packet_free(pack);
		return NULL;
	}

	return 0;
}

static struct packet *udp_create_packet(struct protocol *proto, const struct ipv4_address *src, const struct ipv4_address *dest, const unsigned char *buf, int nbytes)
{
	struct packet *pack;

	pack = packet_alloc(NULL, proto, nbytes + 100);

	if (udp_encode_packet(pack, src, dest, buf, nbytes)) {
		packet_free(pack);
		return NULL;
	}

	return pack;
}


int udp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset)
{
	struct udp_header *hdr;
	struct ipv4_custom_data *custom;

	if (pack->length - offset < sizeof(struct udp_header))
		return -5;

	hdr = (struct udp_header *) &pack->data[offset];
	hdr->src = from_be16(hdr->src);
	hdr->dest = from_be16(hdr->dest);
	hdr->length = from_be16(hdr->length);
	hdr->checksum = from_be16(hdr->checksum);

	if (hdr->length != pack->length - offset)
		return -6;
	pack->transport_offset = offset;
	pack->data_offset = offset + sizeof(struct udp_header);

	// TODO validate checksum

	custom = (struct ipv4_custom_data *) &pack->custom_data;
	custom->src.port = hdr->src;
	custom->dest.port = hdr->dest;

	return 0;
}

int udp_forward_packet(struct protocol *proto, struct packet *pack)
{
	struct endpoint *ep;
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) pack->custom_data;

	ep = lookup_endpoint(proto, custom->dest.addr, custom->dest.port);
	if (!ep)
		return PACKET_DROPPED;

	_queue_insert_after(&ep->recv_queue, &pack->node, ep->recv_queue.tail);
	net_socket_wakeup(ep->sock);
	return PACKET_DELIVERED;
}


static struct endpoint *lookup_endpoint(struct protocol *proto, uint32_t addr, uint16_t port)
{
	for (struct udp_endpoint *cur = _queue_head(&proto->endpoints); cur; cur = _queue_next(&cur->ep.node)) {
		if (cur->src.port == port)
			return (struct endpoint *) cur;
	}
	return NULL;
}

int udp_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result)
{
	int error;
	struct if_device *ifdev;
	struct ipv4_address src;
	struct udp_endpoint *ep;

	// TODO this is a hack.  this should be resolved by the router
	ifdev = net_if_find(NULL, &ipv4_protocol);

	// Get the real IP and port number to send from
	error = inet_resolve_address(sockaddr, len, (const struct sockaddr *) &ifdev->address, &src);
	if (error)
		return error;

	// Make sure the address and port aren't already in use
	if (lookup_endpoint(proto, src.addr, src.port))
		return EADDRINUSE;

	ep = kmalloc(sizeof(struct udp_endpoint));
	_queue_node_init(&ep->ep.node);
	_queue_init(&ep->ep.recv_queue);
	ep->ep.ops = &udp_endpoint_ops;
	ep->ep.proto = proto;
	ep->ep.sock = sock;
	ep->ep.ifdev = ifdev;
	ep->src = src;
	ep->dest.addr = 0;
	ep->dest.port = 0;
	*result = (struct endpoint *) ep;

	_queue_insert_after(&proto->endpoints, &ep->ep.node, proto->endpoints.tail);
	return 0;
}

int udp_destroy_endpoint(struct endpoint *ep)
{
	for (struct packet *next, *cur = _queue_head(&ep->recv_queue); cur; cur = next) {
		next = _queue_next(&cur->node);
		packet_free(cur);
	}
	_queue_init(&ep->recv_queue);

	_queue_remove(&ep->proto->endpoints, &ep->node);
	kmfree(ep);
	return 0;
}

int udp_connect_endpoint(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len)
{
	struct sockaddr_in *addr = (struct sockaddr_in *) sockaddr;

	UDP_ENDPOINT(ep)->dest.addr = addr->sin_addr.s_addr;
	UDP_ENDPOINT(ep)->dest.port = addr->sin_port;
	return NULL;
}

int udp_endpoint_send_to(struct endpoint *ep, const char *buf, int nbytes, const struct sockaddr *sockaddr, socklen_t len)
{
	struct packet *pack;
	struct ipv4_address dest;

	if (!sockaddr && !(UDP_ENDPOINT(ep)->dest.port))
		return ENOTCONN;

	if (sockaddr) {
		dest.addr = ((struct sockaddr_in *) sockaddr)->sin_addr.s_addr;
		dest.port = ((struct sockaddr_in *) sockaddr)->sin_port;
	}
	else
		dest = UDP_ENDPOINT(ep)->dest;

	pack = udp_create_packet(ep->proto, &UDP_ENDPOINT(ep)->src, &dest, buf, nbytes);
	net_if_send_packet(ep->ifdev, pack);
	return nbytes;
}

int udp_endpoint_recv_from(struct endpoint *ep, char *buf, int nbytes, struct sockaddr *sockaddr, socklen_t *len)
{
	struct packet *pack;
	struct ipv4_custom_data *custom;

	pack = _queue_head(&ep->recv_queue);
	if (!pack)
		return EWOULDBLOCK;

	_queue_remove(&ep->recv_queue, &pack->node);

	if (pack->length - pack->data_offset < nbytes)
		nbytes = pack->length - pack->data_offset;
	memcpy(buf, &pack->data[pack->data_offset], nbytes);

	if (sockaddr) {
		custom = (struct ipv4_custom_data *) &pack->custom_data;
		inet_load_sockaddr(sockaddr, len, &custom->src);
	}

	packet_free(pack);
	return nbytes;
}

int udp_endpoint_get_options(struct endpoint *ep, int level, int optname, void *optval, socklen_t *optlen)
{
	switch (optname) {
		case SO_GETSOCKNAME:
			if (!UDP_ENDPOINT(ep)->src.addr)
				return ENOTCONN;
			inet_load_sockaddr((struct sockaddr *) optval, optlen, &UDP_ENDPOINT(ep)->src);
			return 0;
		case SO_GETPEERNAME:
			if (!UDP_ENDPOINT(ep)->dest.addr)
				return ENOTCONN;
			inet_load_sockaddr((struct sockaddr *) optval, optlen, &UDP_ENDPOINT(ep)->dest);
			return 0;
		default:
			return -1;
	}
}

int udp_endpoint_set_options(struct endpoint *ep, int level, int optname, const void *optval, socklen_t optlen)
{
	switch (optname) {
		default:
			return -1;
	}
}

