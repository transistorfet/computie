
#ifndef _SRC_KERNEL_NET_IF_H
#define _SRC_KERNEL_NET_IF_H

#include <stdint.h>
#include <sys/ioc_if.h>

#include "socket.h"
#include "packet.h"


struct if_device;

struct if_ops {
	int (*init)();
	int (*up)(struct if_device *ifdev);
	int (*down)(struct if_device *ifdev);
	int (*poll)(struct if_device *ifdev);
};

struct if_statistics {
	int packets;
	int errors;
	int dropped;
};

struct if_device {
	struct if_ops *ops;

	char *name;
	uint16_t mtu;
	uint16_t flags;

	struct sockaddr_storage address;
	struct protocol *incoming_proto;

	struct queue rx_queue;
	struct queue tx_queue;
	struct if_statistics rx_stats;
	struct if_statistics tx_stats;
};

void init_net_if();
int net_if_register_device(struct if_device *ifdev);
struct if_device *net_if_find(const char *name, struct protocol *proto);
int net_if_change_state(struct if_device *ifdev, int state);
int net_if_ioctl(int request, struct ifreq *ifreq, uid_t uid);
int net_if_send_packet(struct if_device *ifdev, struct packet *pack);
int net_if_recv_packet(struct if_device *ifdev, struct packet **pack);

#endif
