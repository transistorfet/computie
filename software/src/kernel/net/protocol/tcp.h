
#ifndef _SRC_KERNEL_NET_PROTOCOL_TCP_H
#define _SRC_KERNEL_NET_PROTOCOL_TCP_H

#include "../../misc/queue.h"
#include "../../misc/circlebuf.h"

#define TCP_ADDRESS(x)		((struct tcp_address *) (x))
#define TCP_ENDPOINT(x)		((struct tcp_endpoint *) (x))

enum tcp_flag_t {
	NS  = 0x100,
	CWR = 0x080,
	ECE = 0x040,
	URG = 0x020,
	ACK = 0x010,
	PSH = 0x008,
	RST = 0x004,
	SYN = 0x002,
	FIN = 0x001,
};

struct tcp_header {
	uint16_t src;
	uint16_t dest;
	uint32_t seqnum;
	uint32_t acknum;

	uint16_t offset: 4;
	uint16_t flags: 12;
	uint16_t window;
	uint16_t checksum;
	uint16_t urgent;
};


enum tcp_state_t {
	TS_CLOSED = 0,
	TS_LISTEN = 1,
	TS_SYN_SENT = 2,
	TS_SYN_RECV = 3,
	TS_ESTABLISHED = 4,
	TS_FIN_WAIT1 = 5,
	TS_FIN_WAIT2 = 6,
	TS_CLOSE_WAIT = 7,
	TS_CLOSING = 8,
	TS_LAST_ACK = 9,
	TS_TIME_WAIT = 10,
};


struct tcp_endpoint {
	struct endpoint ep;
	struct ipv4_address local;
	struct ipv4_address remote;

	int queue_size;
	struct queue recv_queue;

	int state;
	int listen_queue_max;
	unsigned int tx_last_seq;
	unsigned int rx_last_seq;

	struct circular_buffer *rx;
	struct circular_buffer *tx;
};

extern struct protocol tcp_protocol;

#endif

