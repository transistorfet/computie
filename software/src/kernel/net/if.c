
#include <string.h>
#include <kernel/bh.h>
#include <kernel/driver.h>

// TODO this is temporary until a proper solution for ifup/ip addresses is found
#include <netinet/in.h>

#include "if.h"
#include "packet.h"
#include "protocol.h"

#define IFDEV_MAX	2

static struct if_device *active_ifdevs[IFDEV_MAX];
static void net_if_process_bh(void *_unused);

void init_net_if()
{
	for (char i = 0; i < IFDEV_MAX; i++)
		active_ifdevs[i] = NULL;

	register_bh(BH_NET, net_if_process_bh, NULL);
}

int register_if_device(struct if_device *ifdev)
{
	for (char i = 0; i < IFDEV_MAX; i++) {
		if (!active_ifdevs[i])
			active_ifdevs[i] = ifdev;
	}
}

int net_if_up(const char *name)
{
	for (char i = 0; i < IFDEV_MAX; i++) {
		if (!strcmp(active_ifdevs[i]->name, name)) {
			// TODO a lot more needs to be done here
			extern struct protocol ipv4_protocol;
			active_ifdevs[i]->incoming_proto = &ipv4_protocol;

			((struct sockaddr_in *) &active_ifdevs[i]->address)->sin_family = AF_INET;
			((struct sockaddr_in *) &active_ifdevs[i]->address)->sin_addr.s_addr = 0xC0A801C8;
			((struct sockaddr_in *) &active_ifdevs[i]->address)->sin_port = 0;

			printk_safe("%s: bringing up interface\n", active_ifdevs[i]->name);
			return active_ifdevs[i]->ops->up(active_ifdevs[i]);
		}
	}
	return -1;
}

struct if_device *net_if_find(const char *name)
{
	for (char i = 0; i < IFDEV_MAX; i++) {
		if (!strcmp(active_ifdevs[i]->name, name))
			return active_ifdevs[i];
	}
	return NULL;
}

int net_if_send_packet(struct if_device *ifdev, struct packet *pack)
{
	_queue_insert_after(&ifdev->tx_queue, &pack->node, ifdev->tx_queue.tail);
	ifdev->tx_stats.packets += 1;
	ifdev->ops->poll(ifdev);
	return 0;
}

static void net_if_process_bh(void *_unused)
{
	int error;

	for (short i = 0; i < IFDEV_MAX; i++) {
		if (active_ifdevs[i]->rx_queue.head) {
			struct packet *pack;

			while ((pack = (struct packet *) active_ifdevs[i]->rx_queue.head)) {
				active_ifdevs[i]->rx_stats.packets += 1;
				_queue_remove(&active_ifdevs[i]->rx_queue, &pack->node);

				// If there's no registered protocol, then discard all packets
				if (!active_ifdevs[i]->incoming_proto) {
					active_ifdevs[i]->rx_stats.dropped += 1;
					packet_free(pack);
					continue;
				}

				error = net_incoming_packet(active_ifdevs[i]->incoming_proto, pack);
				if (error == PACKET_DROPPED) {
					printk_safe("packet dropped\n");
					active_ifdevs[i]->rx_stats.dropped += 1;
				}
				else if (error == PACKET_ERROR) {
					printk_safe("packet error\n");
					active_ifdevs[i]->rx_stats.errors += 1;
				}
			}
		}
		// TODO should this also check the tx_queue?
	}
}

