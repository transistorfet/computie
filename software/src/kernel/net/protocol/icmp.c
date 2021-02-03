#include <asm/macros.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../protocol.h"

#include "ipv4.h"


#define ICMP_TYPE_ECHO		8


int icmp_init();
int icmp_encode_header(struct protocol *proto, struct packet *pack, const struct address *src, const struct address *dest, const unsigned char *data, int length);
int icmp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int icmp_forward_packet(struct protocol *proto, struct packet *pack);


struct protocol_ops icmp_protocol_ops = {
	icmp_init,
	icmp_encode_header,
	icmp_decode_header,
	icmp_forward_packet,
	NULL,
};

struct protocol icmp_protocol = {
	&icmp_protocol_ops,
	&ipv4_protocol,
	PF_INET,
	SOCK_RAW,
	IPPROTO_ICMP,
	{ NULL },
};


struct icmp_header {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint32_t data;
};


int icmp_init()
{
	net_register_protocol(&icmp_protocol);
}

int icmp_encode_header(struct protocol *proto, struct packet *pack, const struct address *src, const struct address *dest, const unsigned char *data, int length)
{
	int error;
	struct icmp_header hdr;

	error = proto->next->ops->encode_header(proto->next, pack, src, dest, data, length);
	if (error)
		return error;
	return 0;
}

int icmp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset)
{
	struct icmp_header *hdr;

	if (pack->length - offset < sizeof(struct icmp_header))
		return -1;

	pack->transport_offset = offset;
	pack->data_offset = offset + sizeof(struct icmp_header);

	hdr = (struct icmp_header *) &pack->data[offset];
	hdr->checksum = from_be16(hdr->checksum);
	hdr->data = from_be32(hdr->data);

	// TODO validate checksum

	return 0;
}


int icmp_forward_packet(struct protocol *proto, struct packet *pack)
{
	struct icmp_header *hdr = (struct icmp_header *) &pack->data[pack->transport_offset];
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) pack->custom_data;

	printk_safe("ICMP recevied: %d %d\n", hdr->type, hdr->code);

	switch (hdr->type) {
		case ICMP_TYPE_ECHO: {
			struct packet *reply;
			reply = net_create_packet(proto, (struct address *) &custom->dest, (struct address *) &custom->src, &pack->data[pack->transport_offset], pack->length - pack->transport_offset);

			hdr = (struct icmp_header *) &reply->data[pack->transport_offset];
			hdr->type = 0;
			hdr->code = 0;
			hdr->checksum = 0;
			hdr->checksum = ipv4_calculate_checksum(&hdr, sizeof(struct icmp_header));

			net_if_send_packet(pack->ifdev, reply);
			return PACKET_DELIVERED;
		}
		default:
			return PACKET_DROPPED;
	}
}



