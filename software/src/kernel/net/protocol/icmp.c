
#include <asm/macros.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../protocol.h"

#include "ipv4.h"


#define ICMP_TYPE_ECHO		8


int icmp_init();
int icmp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int icmp_forward_packet(struct protocol *proto, struct packet *pack);


struct protocol_ops icmp_protocol_ops = {
	icmp_init,
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

static int icmp_encode_packet(struct packet *pack, uint8_t type, uint8_t code, struct ipv4_address *src, struct ipv4_address *dest, char *data, int length)
{
	int error;
	struct icmp_header *hdr;

	error = ipv4_encode_header(pack, src, dest, data, length);
	if (error)
		return error;

	hdr = (struct icmp_header *) data;
	hdr->type = type;
	hdr->code = code;
	hdr->checksum = 0;
	hdr->checksum = to_be16(ipv4_calculate_checksum(&hdr, sizeof(struct icmp_header)));

	pack->data_offset = pack->length;
	if (packet_append(pack, data, length))
		return -1;
	return 0;
}

static struct packet *icmp_create_packet(struct protocol *proto, uint8_t type, uint8_t code, struct ipv4_address *src, struct ipv4_address *dest, char *data, int length)
{
	struct packet *pack;

	pack = packet_alloc(NULL, proto, length + 100);

	if (icmp_encode_packet(pack, type, code, src, dest, data, length)) {
		packet_free(pack);
		return NULL;
	}

	return pack;
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
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) pack->custom_data;
	struct icmp_header *hdr = (struct icmp_header *) &pack->data[pack->transport_offset];

	printk_safe("ICMP recevied: %d %d\n", hdr->type, hdr->code);

	switch (hdr->type) {
		case ICMP_TYPE_ECHO: {
			struct packet *reply;

			reply = icmp_create_packet(proto, 0, 0, &custom->dest, &custom->src, &pack->data[pack->transport_offset], pack->length - pack->transport_offset);
			if (!reply)
				return PACKET_DROPPED;

			reply->ifdev = pack->ifdev;
			packet_free(pack);

			net_if_send_packet(reply->ifdev, reply);
			return PACKET_DELIVERED;
		}
		default:
			return PACKET_DROPPED;
	}
}



