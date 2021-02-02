
#include <asm/macros.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../socket.h"
#include "../protocol.h"

extern struct protocol ipv4_protocol;


int udp_init();
int udp_encode_header(struct protocol *proto, struct packet *pack, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *data, int length);
int udp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int udp_forward_packet(struct protocol *proto, struct packet *pack);
int udp_fetch_sockaddr(struct protocol *proto, struct packet *pack, int type, struct sockaddr *sockaddr, socklen_t len);


struct protocol_ops udp_protocol_ops = {
	udp_init,
	udp_encode_header,
	udp_decode_header,
	udp_forward_packet,
	udp_fetch_sockaddr,
};

struct protocol udp_protocol = {
	&udp_protocol_ops,
	&ipv4_protocol,
	PF_INET,
	SOCK_DGRAM,
	IPPROTO_UDP,
	{ NULL },
};


struct udp_header {
	uint16_t src;
	uint16_t dest;
	uint16_t length;
	uint16_t checksum;
};


int udp_init()
{
	register_protocol(&udp_protocol);
}

int udp_encode_header(struct protocol *proto, struct packet *pack, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *data, int length)
{
	int error;
	struct udp_header hdr;

	error = proto->next->ops->encode_header(proto->next, pack, src, dest, data, sizeof(struct udp_header) + length);
	if (error)
		return error;

	hdr.src = to_be16(((struct sockaddr_in *) src)->sin_port);
	hdr.dest = to_be16(((struct sockaddr_in *) dest)->sin_port);
	hdr.length = to_be16(sizeof(struct udp_header) + length);
	hdr.checksum = to_be16(0xFFFF);

	pack->transport_offset = pack->length;
	error = packet_append(pack, &hdr, sizeof(struct udp_header));
	if (error)
		return error;

	return 0;
}

int udp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset)
{
	struct udp_header *hdr;

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

	return 0;
}

int udp_forward_packet(struct protocol *proto, struct packet *pack)
{
	struct socket *sock;
	struct sockaddr_in addr;
	struct udp_header *udphdr;

	memset(&addr, '\0', sizeof(struct sockaddr_in));
	if (proto->ops->fetch_sockaddr(proto, pack, SAT_DEST, &addr, sizeof(struct sockaddr_in)))
		return PACKET_DROPPED;

	sock = find_protocol_listener(proto, &addr, sizeof(struct sockaddr_in));
	if (!sock)
		return PACKET_DROPPED;

	_queue_insert_after(&sock->recv_queue, &pack->node, sock->recv_queue.tail);
	net_socket_wakeup(sock);
	return PACKET_DELIVERED;
}

int udp_fetch_sockaddr(struct protocol *proto, struct packet *pack, int type, struct sockaddr *sockaddr, socklen_t len)
{
	struct udp_header *hdr;

	sockaddr->sa_family = AF_INET;

	hdr = (struct udp_header *) &pack->data[pack->transport_offset];
	if (type == SAT_SRC)
		((struct sockaddr_in *) sockaddr)->sin_port = hdr->src;
	else if (type == SAT_DEST)
		((struct sockaddr_in *) sockaddr)->sin_port = hdr->dest;
	else
		return -1;

	return proto->next->ops->fetch_sockaddr(proto->next, pack, type, sockaddr, len);
}

