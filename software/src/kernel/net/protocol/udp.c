
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

#define UDP_ADDRESS(x)		((struct udp_address *) (x))
#define UDP_ENDPOINT(x)		((struct udp_endpoint *) (x))

extern struct protocol ipv4_protocol;


int udp_init();
int udp_encode_header(struct protocol *proto, struct packet *pack, const struct address *src, const struct address *dest, const unsigned char *data, int length);
int udp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int udp_forward_packet(struct protocol *proto, struct packet *pack);
int udp_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result);

int udp_connect_endpoint(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len);
int udp_destroy_endpoint(struct endpoint *ep);
int udp_endpoint_send_to(struct endpoint *ep, const char *buf, int nbytes, const struct sockaddr *sockaddr, socklen_t len);
int udp_endpoint_recv_from(struct endpoint *ep, char *buf, int nbytes, struct sockaddr *sockaddr, socklen_t *len);


struct udp_address {
	struct ipv4_address addr;
	uint16_t port;
};

struct protocol_ops udp_protocol_ops = {
	udp_init,
	udp_encode_header,
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
	udp_endpoint_send_to,
	udp_endpoint_recv_from,
};

struct udp_endpoint {
	struct endpoint ep;
	struct udp_address src;
	struct udp_address dest;
};

struct udp_custom_data {
	struct ipv4_custom_data ipv4;
	uint16_t src_port;
	uint16_t dest_port;
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

int udp_encode_header(struct protocol *proto, struct packet *pack, const struct address *src, const struct address *dest, const unsigned char *data, int length)
{
	int error;
	struct udp_header hdr;

	error = proto->next->ops->encode_header(proto->next, pack, src, dest, data, sizeof(struct udp_header) + length);
	if (error)
		return error;

	hdr.src = to_be16(((struct udp_address *) src)->port);
	hdr.dest = to_be16(((struct udp_address *) dest)->port);
	hdr.length = to_be16(sizeof(struct udp_header) + length);
	// TODO calculate checksum
	hdr.checksum = to_be16(0);

	pack->transport_offset = pack->length;
	error = packet_append(pack, &hdr, sizeof(struct udp_header));
	if (error)
		return error;

	return 0;
}

int udp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset)
{
	struct udp_header *hdr;
	struct udp_custom_data *custom;

	if (pack->length - offset < sizeof(struct udp_header))
		return -1;

	hdr = (struct udp_header *) &pack->data[offset];
	hdr->src = from_be16(hdr->src);
	hdr->dest = from_be16(hdr->dest);
	hdr->length = from_be16(hdr->length);
	hdr->checksum = from_be16(hdr->checksum);

	if (hdr->length != pack->length - offset)
		return -1;
	pack->transport_offset = offset;
	pack->data_offset = offset + sizeof(struct udp_header);

	// TODO validate checksum

	custom = (struct udp_custom_data *) &pack->custom_data;
	custom->src_port = hdr->src;
	custom->dest_port = hdr->dest;

	return 0;
}

int udp_forward_packet(struct protocol *proto, struct packet *pack)
{
	struct endpoint *ep;
	struct udp_custom_data *custom = (struct udp_custom_data *) pack->custom_data;

	printk("forwarding %x:%d\n", custom->ipv4.dest.addr, custom->dest_port);
	ep = lookup_endpoint(proto, custom->ipv4.dest.addr, custom->dest_port);
	if (!ep)
		return PACKET_DROPPED;

	printk("found endpoint\n");
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
	struct udp_address src;
	struct udp_endpoint *ep;
	struct if_device *ifdev;
	struct sockaddr_in *sa = (struct sockaddr_in *) sockaddr;

	if (len != sizeof(struct sockaddr_in))
		return EINVAL;

	// TODO this is a hack for now
	ifdev = net_if_find("slip0");

	if (sa->sin_addr.s_addr != INADDR_ANY && sa->sin_addr.s_addr != ((struct sockaddr_in *) &ifdev->address)->sin_addr.s_addr)
		return EINVAL;

	// Get the actual IP and port to send from
	src.addr.addr = sa->sin_addr.s_addr;
	src.port = sa->sin_port;
	if (src.addr.addr == INADDR_ANY)
		src.addr.addr = ((struct sockaddr_in *) &ifdev->address)->sin_addr.s_addr;
	if (src.port == 0)
		// TODO this should be an ephemeral port.  This is a temporary hack
		src.port = 1284;

	printk_safe("%x %d\n", src.addr.addr, src.port);

	// Make sure this address and port aren't already in use
	if (lookup_endpoint(proto, src.addr.addr, src.port))
		return EADDRINUSE;

	ep = kmalloc(sizeof(struct udp_endpoint));
	_queue_node_init(&ep->ep.node);
	_queue_init(&ep->ep.recv_queue);
	ep->ep.ops = &udp_endpoint_ops;
	ep->ep.proto = proto;
	ep->ep.sock = sock;
	ep->ep.ifdev = ifdev;
	ep->src = src;
	ep->dest.addr.addr = 0;
	ep->dest.port = 0;
	*result = (struct endpoint *) ep;

	_queue_insert_after(&proto->endpoints, &ep->ep.node, proto->endpoints.tail);
	return 0;
}

int udp_destroy_endpoint(struct endpoint *ep)
{
	printk("destroying endpoint\n");
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

	UDP_ENDPOINT(ep)->dest.addr.addr = addr->sin_addr.s_addr;
	UDP_ENDPOINT(ep)->dest.port = addr->sin_port;
	return NULL;
}

int udp_endpoint_send_to(struct endpoint *ep, const char *buf, int nbytes, const struct sockaddr *sockaddr, socklen_t len)
{
	struct packet *pack;
	struct udp_address dest;

	if (!sockaddr && !(UDP_ENDPOINT(ep)->dest.port))
		return ENOTCONN;

	if (sockaddr) {
		dest.addr.addr = ((struct sockaddr_in *) sockaddr)->sin_addr.s_addr;
		dest.port = ((struct sockaddr_in *) sockaddr)->sin_port;
	}
	else
		dest = UDP_ENDPOINT(ep)->dest;

	pack = net_create_packet(ep->proto, (struct address *) &UDP_ENDPOINT(ep)->src, (struct address *) &dest, buf, nbytes);
	net_if_send_packet(ep->ifdev, pack);
	return nbytes;
}

int udp_endpoint_recv_from(struct endpoint *ep, char *buf, int nbytes, struct sockaddr *sockaddr, socklen_t *len)
{
	struct packet *pack;
	struct udp_custom_data *custom = (struct udp_custom_data *) pack->custom_data;

	pack = _queue_head(&ep->recv_queue);
	if (!pack)
		return EWOULDBLOCK;

	_queue_remove(&ep->recv_queue, &pack->node);

	if (pack->length - pack->data_offset < nbytes)
		nbytes = pack->length - pack->data_offset;
	memcpy(buf, &pack->data[pack->data_offset], nbytes);

	if (sockaddr) {
		struct sockaddr_in *addr = (struct sockaddr_in *) sockaddr;
		memset(addr, '\0', *len);
		addr->sin_family = AF_INET;
		addr->sin_port = custom->src_port;
		addr->sin_addr.s_addr = custom->ipv4.src.addr;
	}

	packet_free(pack);
	return nbytes;
}

