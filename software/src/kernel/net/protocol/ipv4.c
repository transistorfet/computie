
#include <asm/macros.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../protocol.h"


int ipv4_init();
int ipv4_encode_header(struct protocol *proto, struct packet *pack, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *data, int length);
int ipv4_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int ipv4_forward_packet(struct protocol *proto, struct packet *pack);
int ipv4_fetch_sockaddr(struct protocol *proto, struct packet *pack, int type, struct sockaddr *sockaddr, socklen_t len);


struct protocol_ops ipv4_protocol_ops = {
	ipv4_init,
	ipv4_encode_header,
	ipv4_decode_header,
	ipv4_forward_packet,
	ipv4_fetch_sockaddr,
};

struct protocol ipv4_protocol = {
	&ipv4_protocol_ops,
	NULL,
	PF_INET,
	SOCK_RAW,
	IPPROTO_IP,
	{ NULL },
};


struct ipv4_header {
	uint8_t version: 4;
	uint8_t ihl: 4;
	uint8_t dscp: 6;
	uint8_t ecn: 2;
	uint16_t length;
	uint16_t id;
	uint16_t flags: 3;
	uint16_t frag_offset: 13;

	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;

	uint32_t src;
	uint32_t dest;
};

int ipv4_init()
{
	register_protocol(&ipv4_protocol);
}

int ipv4_encode_header(struct protocol *proto, struct packet *pack, const struct sockaddr *src, const struct sockaddr *dest, const unsigned char *data, int length)
{
	struct ipv4_header hdr;

	hdr.version = 4;
	hdr.ihl = 5;
	hdr.dscp = 0;
	hdr.ecn = 0;
	hdr.length = to_be16(sizeof(struct ipv4_header) + length);
	hdr.id = to_be16(0);
	hdr.flags = 2;
	hdr.frag_offset = to_be16(0);
	hdr.ttl = 0x40;
	hdr.protocol = pack->protocol;
	hdr.checksum = to_be16(0xFFFF);
	hdr.src = to_be32(((struct sockaddr_in *) src)->sin_addr.s_addr);
	hdr.dest = to_be32(((struct sockaddr_in *) dest)->sin_addr.s_addr);

	pack->network_offset = pack->length;
	return packet_append(pack, &hdr, sizeof(struct ipv4_header));
}

int ipv4_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset)
{
	int error;
	struct protocol *next;
	struct ipv4_header *hdr;

	pack->network_offset = offset;
	hdr = (struct ipv4_header *) &pack->data[offset];

	if (pack->length - offset < sizeof(struct ipv4_header))
		return -1;
	pack->data_offset = offset + sizeof(struct ipv4_header);
	if (hdr->version != 4 || hdr->ihl < 5 || hdr->ihl > 9)
		return -1;

	hdr->length = from_be16(hdr->length);
	hdr->checksum = from_be16(hdr->checksum);
	hdr->src = from_be32(hdr->src);
	hdr->dest = from_be32(hdr->dest);

	// TODO validate checksum

	pack->protocol = hdr->protocol;
	next = get_protocol(PF_INET, 0, hdr->protocol);
	if (!next)
		return -2;

	error = next->ops->decode_header(next, pack, offset + (((uint16_t) hdr->ihl) << 2));
	if (error)
		return error;

	return 0;
}


int ipv4_forward_packet(struct protocol *proto, struct packet *pack)
{
	struct protocol *next;

	next = get_protocol(PF_INET, 0, pack->protocol);
	if (!next)
		return PACKET_DROPPED;
	return next->ops->forward_packet(next, pack);
}

int ipv4_fetch_sockaddr(struct protocol *proto, struct packet *pack, int type, struct sockaddr *sockaddr, socklen_t len)
{
	struct ipv4_header *hdr;

	hdr = (struct ipv4_header *) &pack->data[pack->network_offset];
	if (type == SAT_SRC)
		((struct sockaddr_in *) sockaddr)->sin_addr.s_addr = hdr->src;
	else if (type == SAT_DEST)
		((struct sockaddr_in *) sockaddr)->sin_addr.s_addr = hdr->dest;
	else
		return -1;

	return 0;
}

