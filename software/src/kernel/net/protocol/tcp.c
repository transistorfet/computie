
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <asm/macros.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <kernel/kmalloc.h>

#include "../../misc/queue.h"

#include "../if.h"
#include "../socket.h"
#include "../protocol.h"

#include "ipv4.h"
#include "inet_af.h"

#include "tcp.h"


#define TCP_RX_BUFFER		1024
#define TCP_TX_BUFFER		1024


int tcp_init();
int tcp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset);
int tcp_forward_packet(struct protocol *proto, struct packet *pack);
int tcp_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result);

int tcp_destroy_endpoint(struct endpoint *ep);
int tcp_endpoint_listen(struct endpoint *ep, int queue);
int tcp_endpoint_accept(struct endpoint *ep, struct sockaddr *sockaddr, socklen_t *len, struct endpoint **result);
int tcp_endpoint_connect(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len);
int tcp_endpoint_shutdown(struct endpoint *ep, int how);
int tcp_endpoint_send(struct endpoint *ep, const char *buf, int nbytes);
int tcp_endpoint_recv(struct endpoint *ep, char *buf, int nbytes);
int tcp_endpoint_get_options(struct endpoint *ep, int level, int optname, void *optval, socklen_t *optlen);
int tcp_endpoint_set_options(struct endpoint *ep, int level, int optname, const void *optval, socklen_t optlen);
int tcp_endpoint_poll(struct endpoint *ep, int events);


struct protocol_ops tcp_protocol_ops = {
	tcp_init,
	tcp_decode_header,
	tcp_forward_packet,
	tcp_create_endpoint,
};

struct protocol tcp_protocol = {
	&tcp_protocol_ops,
	&ipv4_protocol,
	PF_INET,
	SOCK_STREAM,
	IPPROTO_TCP,
};

struct endpoint_ops tcp_endpoint_ops = {
	tcp_destroy_endpoint,
	tcp_endpoint_listen,
	tcp_endpoint_accept,
	tcp_endpoint_connect,
	tcp_endpoint_shutdown,
	tcp_endpoint_send,
	tcp_endpoint_recv,
	NULL,
	NULL,
	tcp_endpoint_get_options,
	tcp_endpoint_set_options,
	tcp_endpoint_poll,
};


static struct tcp_endpoint *tcp_alloc_endpoint(struct protocol *proto, struct socket *sock, struct if_device *ifdev, struct ipv4_address *local, struct ipv4_address *remote);
static struct endpoint *tcp_lookup_endpoint(struct protocol *proto, struct ipv4_address *local, struct ipv4_address *remote);
static int tcp_create_client_endpoint(struct tcp_endpoint *tep, struct packet *pack, struct endpoint **result);
static struct packet *tcp_create_empty_packet(struct protocol *proto, const struct ipv4_address *src, const struct ipv4_address *dest, int nbytes);
static int tcp_setup_packet(struct packet *pack, const struct ipv4_address *src, const struct ipv4_address *dest, int length);
static uint16_t tcp_calculate_checksum(struct protocol *proto, struct packet *pack);
static int tcp_finalize_and_send_packet(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_send_packet(struct tcp_endpoint *tep, int flags, int nbytes, int ackbytes);
static void tcp_queue_packet(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_check_waiting_packet(struct tcp_endpoint *tep);

static int tcp_forward_listen(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_forward_syn_sent(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_forward_syn_recv(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_forward_established(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_forward_closing(struct tcp_endpoint *tep, struct packet *pack);
static int tcp_check_close(struct tcp_endpoint *tep, struct packet *pack);

static struct queue tcp_endpoints;


/*******************************
 *  Public Protocol Functions  *
 *******************************/

int tcp_init()
{
	_queue_init(&tcp_endpoints);
	net_register_protocol(&tcp_protocol);
}

int tcp_decode_header(struct protocol *proto, struct packet *pack, uint16_t offset)
{
	uint16_t checksum;
	struct tcp_header *hdr;
	struct ipv4_custom_data *custom;

	if (pack->length - offset < sizeof(struct tcp_header))
		return -5;

	hdr = (struct tcp_header *) &pack->data[offset];
	hdr->src = from_be16(hdr->src);
	hdr->dest = from_be16(hdr->dest);
	hdr->seqnum = from_be32(hdr->seqnum);
	hdr->acknum = from_be32(hdr->acknum);
	hdr->flags = from_be16(hdr->flags);
	hdr->window = from_be16(hdr->window);
	hdr->checksum = from_be16(hdr->checksum);
	hdr->urgent = from_be16(hdr->urgent);

	pack->transport_offset = offset;
	pack->data_offset = offset + (hdr->offset << 2);

	custom = (struct ipv4_custom_data *) &pack->custom_data;
	custom->src.port = hdr->src;
	custom->dest.port = hdr->dest;

	checksum = hdr->checksum;
	hdr->checksum = 0;
	if (checksum != tcp_calculate_checksum(proto, pack))
		return -7;
	hdr->checksum = checksum;

	return 0;
}

int tcp_forward_packet(struct protocol *proto, struct packet *pack)
{
	struct endpoint *ep;
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) pack->custom_data;

	ep = tcp_lookup_endpoint(proto, &custom->dest, &custom->src);
	if (!ep)
		return PACKET_DROPPED;

	switch (TCP_ENDPOINT(ep)->state) {
		case TS_CLOSED:
			return PACKET_DROPPED;
		case TS_LISTEN:
			return tcp_forward_listen(TCP_ENDPOINT(ep), pack);
		case TS_SYN_SENT:
			return tcp_forward_syn_sent(TCP_ENDPOINT(ep), pack);
		case TS_SYN_RECV:
			return tcp_forward_syn_recv(TCP_ENDPOINT(ep), pack);
		case TS_ESTABLISHED:
			return tcp_forward_established(TCP_ENDPOINT(ep), pack);
		case TS_FIN_WAIT1:
		case TS_FIN_WAIT2:
		case TS_CLOSE_WAIT:
		case TS_CLOSING:
		case TS_LAST_ACK:
		case TS_TIME_WAIT:
			return tcp_forward_closing(TCP_ENDPOINT(ep), pack);
		default:
			return PACKET_DROPPED;
	}
}

int tcp_create_endpoint(struct protocol *proto, struct socket *sock, const struct sockaddr *sockaddr, socklen_t len, struct endpoint **result)
{
	int error;
	struct if_device *ifdev;
	struct ipv4_address src;
	struct tcp_endpoint *tep;

	// TODO this is a hack.  this should be resolved by the router
	ifdev = net_if_find(NULL, &ipv4_protocol);

	// Get the real IP and port number to send from
	error = inet_resolve_address(sockaddr, len, (const struct sockaddr *) &ifdev->address, &src);
	if (error)
		return error;

	// Make sure the address and port aren't already in use
	if (tcp_lookup_endpoint(proto, &src, NULL))
		return EADDRINUSE;

	tep = tcp_alloc_endpoint(proto, sock, ifdev, &src, NULL);
	if (!tep)
		return ENOMEM;
	*result = (struct endpoint *) tep;

	return 0;
}

/*******************************
 *  Public Endpoint Functions  *
 *******************************/

int tcp_destroy_endpoint(struct endpoint *ep)
{
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	// TODO this would need to close properly or return an error??
	if (tep->remote.addr && tep->state != TS_CLOSED && tep->state != TS_CLOSING && !tep->fin_sent)
		tcp_send_packet(tep, RST | ACK, 0, 0);

	for (struct packet *next, *cur = _queue_head(&tep->recv_queue); cur; cur = next) {
		next = _queue_next(&cur->node);
		packet_free(cur);
	}
	_queue_init(&tep->recv_queue);

	_queue_remove(&tcp_endpoints, &ep->node);

	if (tep->rx)
		kmfree(tep->rx);
	if (tep->tx)
		kmfree(tep->tx);
	kmfree(tep);
	return 0;
}

int tcp_endpoint_listen(struct endpoint *ep, int queue)
{
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	if (tep->state != TS_CLOSED)
		return EISCONN;

	tep->listen_queue_max = queue < 10 ? queue : 10;
	tep->state = TS_LISTEN;
	return 0;
}

int tcp_endpoint_accept(struct endpoint *ep, struct sockaddr *sockaddr, socklen_t *len, struct endpoint **result)
{
	int error;
	struct packet *pack;
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	if (tep->state != TS_LISTEN)
		return EINVAL;

	pack = _queue_head(&tep->recv_queue);
	if (!pack)
		return EWOULDBLOCK;

	_queue_remove(&tep->recv_queue, &pack->node);
	tep->queue_size -= 1;

	error = tcp_create_client_endpoint(tep, pack, result);
	packet_free(pack);
	if (error)
		return error;

	if (sockaddr)
		inet_load_sockaddr(sockaddr, len, &TCP_ENDPOINT(*result)->remote);
	return 0;
}

int tcp_endpoint_connect(struct endpoint *ep, const struct sockaddr *sockaddr, socklen_t len)
{
	struct packet *pack;
	struct tcp_header *hdr;
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);
	struct sockaddr_in *addr = (struct sockaddr_in *) sockaddr;

	if (tep->state == TS_ESTABLISHED && tep->connect_return != 0)
		return tep->connect_return < 0 ? tep->connect_return : 0;

	if (tep->state != TS_CLOSED)
		return EISCONN;

	tep->remote.addr = addr->sin_addr.s_addr;
	tep->remote.port = addr->sin_port;
	tep->tx_last_seq = rand();
	tep->tx_acks_repeated = 0;

	tcp_send_packet(tep, SYN, 0, 1);
	tep->state = TS_SYN_SENT;

	return EWOULDBLOCK;
}

int tcp_endpoint_shutdown(struct endpoint *ep, int how)
{
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	if (tep->state != TS_SYN_RECV && tep->state != TS_ESTABLISHED && tep->state != TS_CLOSE_WAIT)
		return 0;

	tep->fin_sent = 1;
	tcp_send_packet(tep, FIN | ACK, 0, 0);
	if (tep->state == TS_CLOSE_WAIT)
		tep->state = TS_LAST_ACK;
	else
		tep->state = TS_FIN_WAIT1;
	return 0;
}

int tcp_endpoint_send(struct endpoint *ep, const char *buf, int nbytes)
{
	struct packet *pack;
	struct tcp_header *hdr;
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	if (!(tep->remote.port))
		return ENOTCONN;

	if (tep->state != TS_ESTABLISHED || _buf_free_space(tep->tx) < nbytes)
		return EWOULDBLOCK;

	_buf_put(tep->tx, buf, nbytes);
	tep->tx_last_seq += nbytes;

	tcp_send_packet(tep, PSH | ACK, nbytes, 0);

	return nbytes;
}

int tcp_endpoint_recv(struct endpoint *ep, char *buf, int nbytes)
{
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	if (tep->state == TS_CLOSED)
		return 0;
	if (tep->state == TS_LISTEN)
		return EINVAL;
	if (tep->state == TS_SYN_SENT || tep->state == TS_SYN_RECV)
		return EWOULDBLOCK;

	nbytes = _buf_get(tep->rx, buf, nbytes);
	if (nbytes == 0 && !tep->fin_recv)
		return EWOULDBLOCK;
	return nbytes;
}

int tcp_endpoint_get_options(struct endpoint *ep, int level, int optname, void *optval, socklen_t *optlen)
{
	switch (optname) {
		case SO_GETSOCKNAME:
			if (!TCP_ENDPOINT(ep)->local.addr)
				return ENOTCONN;
			inet_load_sockaddr((struct sockaddr *) optval, optlen, &TCP_ENDPOINT(ep)->local);
			return 0;
		case SO_GETPEERNAME:
			if (!TCP_ENDPOINT(ep)->remote.addr)
				return ENOTCONN;
			inet_load_sockaddr((struct sockaddr *) optval, optlen, &TCP_ENDPOINT(ep)->remote);
			return 0;
		default:
			return -1;
	}
}

int tcp_endpoint_set_options(struct endpoint *ep, int level, int optname, const void *optval, socklen_t optlen)
{
	switch (optname) {
		default:
			return -1;
	}
}

int tcp_endpoint_poll(struct endpoint *ep, int events)
{
	int revents = 0;
	struct tcp_endpoint *tep = TCP_ENDPOINT(ep);

	if (events & VFS_POLL_READ) {
		if (
		    (tep->state == TS_ESTABLISHED && !_buf_is_empty(tep->rx))
		 || (tep->state == TS_LISTEN && _queue_head(&tep->recv_queue))
		 || (tep->state == TS_CLOSED || tep->state >= TS_FIN_WAIT1)
		)
			revents |= VFS_POLL_READ;
	}
	if ((events & VFS_POLL_WRITE) && !_buf_is_full(tep->tx))
		revents |= VFS_POLL_WRITE;
	// TODO add error events
	return revents;
}

/*********************************
 *  Internal Endpoint Functions  *
 *********************************/

static struct tcp_endpoint *tcp_alloc_endpoint(struct protocol *proto, struct socket *sock, struct if_device *ifdev, struct ipv4_address *local, struct ipv4_address *remote)
{
	struct tcp_endpoint *tep;

	tep = kmalloc(sizeof(struct tcp_endpoint));
	memset(tep, '\0', sizeof(struct tcp_endpoint));
	_queue_node_init(&tep->ep.node);
	tep->ep.ops = &tcp_endpoint_ops;
	tep->ep.proto = proto;
	tep->ep.sock = sock;
	tep->ep.ifdev = ifdev;

	tep->local = *local;
	if (remote)
		tep->remote = *remote;
	else {
		tep->remote.addr = 0;
		tep->remote.port = 0;
	}

	_queue_init(&tep->recv_queue);
	tep->queue_size = 0;
	tep->state = TS_CLOSED;
	tep->fin_sent = 0;
	tep->fin_recv = 0;
	tep->listen_queue_max = 0;
	tep->connect_return = 0;
	tep->tx_acks_repeated = 0;

	tep->rx = kmalloc(TCP_RX_BUFFER);
	_buf_init(tep->rx, TCP_RX_BUFFER);
	tep->tx = kmalloc(TCP_TX_BUFFER);
	_buf_init(tep->tx, TCP_TX_BUFFER);

	// Inserting the client endpoints before listeners ensures the client will be found first during lookup
	_queue_insert(&tcp_endpoints, &tep->ep.node);
	return tep;
}

static struct endpoint *tcp_lookup_endpoint(struct protocol *proto, struct ipv4_address *local, struct ipv4_address *remote)
{
	for (struct tcp_endpoint *cur = _queue_head(&tcp_endpoints); cur; cur = _queue_next(&cur->ep.node)) {
		if (cur->local.port == local->port && (cur->state == TS_LISTEN || (remote && cur->remote.addr == remote->addr && cur->remote.port == remote->port)))
			return (struct endpoint *) cur;
	}
	return NULL;
}

static int tcp_create_client_endpoint(struct tcp_endpoint *tep, struct packet *pack, struct endpoint **result)
{
	struct tcp_endpoint *client_ep;
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) pack->custom_data;
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	client_ep = tcp_alloc_endpoint(tep->ep.proto, NULL, pack->ifdev, &tep->local, &custom->src);
	if (!client_ep)
		return ENOMEM;

	client_ep->state = TS_SYN_RECV;
	client_ep->rx_last_seq = hdr->seqnum;
	client_ep->tx_last_seq = rand();

	*result = (struct endpoint *) client_ep;

	tcp_send_packet(client_ep, SYN | ACK, 0, 1);
	return 0;
}

static struct packet *tcp_create_empty_packet(struct protocol *proto, const struct ipv4_address *src, const struct ipv4_address *dest, int nbytes)
{
	struct packet *pack;

	// TODO this packet size really needs to be fixed
	pack = packet_alloc(NULL, proto, nbytes + 100);

	if (tcp_setup_packet(pack, src, dest, nbytes)) {
		packet_free(pack);
		return NULL;
	}

	return pack;
}

static int tcp_setup_packet(struct packet *pack, const struct ipv4_address *src, const struct ipv4_address *dest, int length)
{
	int error;
	struct tcp_header *hdr;

	// TODO the NULL is data, but ipv4_encode doesn't use it
	error = ipv4_encode_header(pack, src, dest, NULL, sizeof(struct tcp_header) + length);
	if (error)
		return error;

	pack->transport_offset = pack->length;
	pack->length += sizeof(struct tcp_header);

	hdr = (struct tcp_header *) &pack->data[pack->transport_offset];
	memset(hdr, '\0', sizeof(struct tcp_header));
	hdr->src = src->port;
	hdr->dest = dest->port;
	hdr->offset = 5;
	hdr->checksum = 0;

	pack->data_offset = pack->length;

	return 0;
}

static uint16_t tcp_calculate_checksum(struct protocol *proto, struct packet *pack)
{
	uint32_t checksum = 0;
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) &pack->custom_data;

	checksum += (custom->src.addr >> 16) + (custom->src.addr & 0xFFFF);
	checksum += (custom->dest.addr >> 16) + (custom->dest.addr & 0xFFFF);
	checksum += proto->protocol;
	checksum += pack->length - pack->transport_offset;

	return ipv4_calculate_checksum(&pack->data[pack->transport_offset], pack->length - pack->transport_offset, checksum);
}

static int tcp_finalize_and_send_packet(struct tcp_endpoint *tep, struct packet *pack)
{
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	// Convert endianness of data in the header
	hdr->src = to_be16(hdr->src);
	hdr->dest = to_be16(hdr->dest);
	hdr->seqnum = to_be32(hdr->seqnum);
	hdr->acknum = to_be32(hdr->acknum);
	hdr->window = to_be16(65535);
	hdr->checksum = to_be16(0xFFFF);
	hdr->urgent = to_be16(hdr->urgent);

	// Update the checksum
	hdr->checksum = to_be16(tcp_calculate_checksum(tep->ep.proto, pack));

	return net_if_send_packet(tep->ep.ifdev, pack);
}

static int tcp_send_packet(struct tcp_endpoint *tep, int flags, int nbytes, int ackbytes)
{
	struct packet *pack;
	struct tcp_header *hdr;

	// TODO there needs to be a limit on how big packets can get.  But should the splitting happen here or in the caller?
	if ((flags & PSH) && nbytes == -1)
		nbytes = _buf_used_space(tep->tx);

	pack = tcp_create_empty_packet(tep->ep.proto, &tep->local, &tep->remote, nbytes);
	hdr = (struct tcp_header *) &pack->data[pack->transport_offset];
	hdr->flags = flags;

	if (flags & ACK) {
		tep->rx_last_seq += ackbytes;
		hdr->acknum = tep->rx_last_seq;
	}

	if (flags & PSH) {
		hdr->seqnum = tep->tx_last_seq - nbytes;
		_buf_peek(tep->tx, &pack->data[pack->length], _buf_used_space(tep->tx) - nbytes, nbytes);
		pack->length += nbytes;
	}
	else {
		hdr->seqnum = tep->tx_last_seq;
		if (flags == SYN || (flags & FIN))
			tep->tx_last_seq += 1;
	}

	return tcp_finalize_and_send_packet(tep, pack);
}

static void tcp_queue_packet(struct tcp_endpoint *tep, struct packet *pack)
{
	unsigned int seq;
	struct packet *cur, *prev;
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	seq = hdr->seqnum;
	for (cur = _queue_head(&tep->recv_queue), prev = NULL; cur; prev = (struct packet *) cur, cur = _queue_next(&cur->node)) {
		hdr = (struct tcp_header *) &cur->data[cur->transport_offset];
		if (seq < hdr->seqnum)
			break;
	}

	_queue_insert_after(&tep->recv_queue, &pack->node, &prev->node);
}

static int tcp_check_waiting_packet(struct tcp_endpoint *tep)
{
	int index;
	int length;
	struct packet *next;
	struct tcp_header *hdr;

	for (struct packet *cur = _queue_head(&tep->recv_queue); cur; cur = next) {
		next = _queue_next(&cur->node);

		hdr = (struct tcp_header *) &cur->data[cur->transport_offset];
		if (tep->rx_last_seq < hdr->seqnum)
			break;

		length = cur->length - cur->data_offset;
		if (hdr->seqnum + length > tep->rx_last_seq) {
			index = tep->rx_last_seq - hdr->seqnum;
			length -= index;

			index = _buf_put(tep->rx, &cur->data[cur->data_offset + index], length);
			tep->rx_last_seq += index;
			if (index < length)
				return 0;
		}
		_queue_remove(&tep->recv_queue, &cur->node);
		packet_free(cur);
	}
	return 1;
}


static int tcp_forward_listen(struct tcp_endpoint *tep, struct packet *pack)
{
	struct tcp_endpoint *client_ep;
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];
	struct ipv4_custom_data *custom = (struct ipv4_custom_data *) pack->custom_data;

	if (hdr->flags != SYN)
		return PACKET_DROPPED;

	if (tep->queue_size >= tep->listen_queue_max)
		// TODO probably send a connection refused message
		return PACKET_DROPPED;

	_queue_insert_after(&tep->recv_queue, &pack->node, tep->recv_queue.tail);
	tep->queue_size += 1;
	net_socket_wakeup(tep->ep.sock, VFS_POLL_READ);

	return PACKET_DELIVERED;
}

static int tcp_forward_syn_recv(struct tcp_endpoint *tep, struct packet *pack)
{
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	// TODO since this is server-side, we need to possible destroy the endpoint that was created by listen?
	if (tcp_check_close(tep, pack) == PACKET_DELIVERED)
		return PACKET_DELIVERED;

	if (hdr->flags != ACK)
		return PACKET_DROPPED;
	// If the acknum of the packet doesn't match our expected seqnum, then drop the packet
	if (hdr->seqnum != tep->rx_last_seq)
		return PACKET_DROPPED;
	tep->tx_last_seq = hdr->acknum;
	tep->tx_acks_repeated = 0;

	tep->state = TS_ESTABLISHED;
	net_socket_wakeup(tep->ep.sock, VFS_POLL_READ);

	packet_free(pack);
	return PACKET_DELIVERED;
}


static int tcp_forward_syn_sent(struct tcp_endpoint *tep, struct packet *pack)
{
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	if (tcp_check_close(tep, pack) == PACKET_DELIVERED)
		return PACKET_DELIVERED;

	if (hdr->flags != (SYN | ACK))
		return PACKET_DROPPED;
	// If the acknum of the packet doesn't match our expected seqnum, then drop the packet
	if (hdr->acknum != tep->tx_last_seq)
		return PACKET_DROPPED;

	tep->rx_last_seq = hdr->seqnum;
	tep->state = TS_ESTABLISHED;
	packet_free(pack);

	tcp_send_packet(tep, ACK, 0, 1);
	tep->connect_return = 1;
	net_socket_wakeup(tep->ep.sock, VFS_POLL_READ);

	return PACKET_DELIVERED;
}

static int tcp_forward_established(struct tcp_endpoint *tep, struct packet *pack)
{
	struct packet *reply;
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	if (tcp_check_close(tep, pack) == PACKET_DELIVERED)
		return PACKET_DELIVERED;

	if (hdr->flags & ACK) {
		// Discard the data that's been acknowledged
		int last_acked = tep->tx_last_seq - _buf_used_space(tep->tx);
		if (hdr->acknum > last_acked) {
			tep->tx_acks_repeated = 0;
			_buf_drop(tep->tx, hdr->acknum - last_acked);
			last_acked = hdr->acknum;
			net_socket_wakeup(tep->ep.sock, VFS_POLL_WRITE);
		}

		// If not all the data sent was acknowledged, retransmit the missing data
		if (last_acked < tep->tx_last_seq) {
			tep->tx_acks_repeated += 1;
			if (tep->tx_acks_repeated >= 3) {
				tep->tx_acks_repeated = 0;
				tcp_send_packet(tep, PSH | ACK, -1, 0);
			}
		}
	}

	if (pack->length - pack->data_offset > 0) {
		tcp_queue_packet(tep, pack);
		if (tcp_check_waiting_packet(tep))
			tcp_send_packet(tep, ACK, 0, 0);

		if (tep->ep.sock && !_buf_is_empty(tep->rx))
			net_socket_wakeup(tep->ep.sock, VFS_POLL_READ);
		return PACKET_DELIVERED;
	}

	packet_free(pack);
	return PACKET_DELIVERED;
}

static int tcp_forward_closing(struct tcp_endpoint *tep, struct packet *pack)
{
	// For now, this is the same behaviour as TS_CLOSING
	if (tcp_check_close(tep, pack) == PACKET_DELIVERED)
		return PACKET_DELIVERED;
	return PACKET_DROPPED;
}

static int tcp_check_close(struct tcp_endpoint *tep, struct packet *pack)
{
	int ret = PACKET_DROPPED;
	struct tcp_header *hdr = (struct tcp_header *) &pack->data[pack->transport_offset];

	// If the other end requests we close the connection (ie. remote port not listening), then close and drop
	if (hdr->flags & RST) {
		tep->fin_recv = 1;
		tep->state = TS_CLOSING;
		tcp_send_packet(tep, RST | ACK, 0, 1);
		net_socket_wakeup(tep->ep.sock, VFS_POLL_READ);
		ret = PACKET_DELIVERED;
	}

	if (hdr->flags & FIN) {
		tep->fin_recv = 1;
		if (hdr->seqnum != tep->rx_last_seq)
			return PACKET_DROPPED;

		if (tep->state == TS_ESTABLISHED)
			tep->state = TS_CLOSE_WAIT;
		else
			tep->state = TS_CLOSING;

		tcp_send_packet(tep, ACK, 0, 1);
		net_socket_wakeup(tep->ep.sock, VFS_POLL_READ);
		ret = PACKET_DELIVERED;
	}

	if (tep->fin_sent && (hdr->flags & ACK) && (hdr->acknum == tep->tx_last_seq)) {
		tep->state = TS_CLOSED;
		ret = PACKET_DELIVERED;
	}

	if (ret == PACKET_DELIVERED)
		packet_free(pack);

	return ret;
}

