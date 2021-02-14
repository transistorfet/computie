
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#include <kernel/bh.h>
#include <kernel/driver.h>
#include <kernel/printk.h>

#include "if.h"
#include "packet.h"
#include "protocol.h"

#define IFDEV_MAX	2

#define IFF_SET_MASK	(IFF_UP | IFF_DEBUG)


static struct if_device *active_ifdevs[IFDEV_MAX];
static void net_if_process_bh(void *_unused);

void init_net_if()
{
	for (char i = 0; i < IFDEV_MAX; i++)
		active_ifdevs[i] = NULL;

	register_bh(BH_NET, net_if_process_bh, NULL);
	enable_bh(BH_NET);
}

int net_if_register_device(struct if_device *ifdev)
{
	for (char i = 0; i < IFDEV_MAX; i++) {
		if (!active_ifdevs[i])
			active_ifdevs[i] = ifdev;
	}
}

struct if_device *net_if_find(const char *name, struct protocol *proto)
{
	for (char i = 0; i < IFDEV_MAX; i++) {
		if ((!name || !strcmp(active_ifdevs[i]->name, name)) && (!proto || active_ifdevs[i]->incoming_proto == proto))
			return active_ifdevs[i];
	}
	return NULL;
}

int net_if_change_state(struct if_device *ifdev, int state)
{
	int error;

	// If the UP state doesn't change, just set some flags and return
	if (!((ifdev->flags & IFF_UP) ^ (state & IFF_UP))) {
		ifdev->flags = state & IFF_SET_MASK;
		return 0;
	}

	if (state & IFF_UP) {
		if (!ifdev->address.ss_family)
			// TODO what error should be here?
			return -1;

		extern struct protocol ipv4_protocol;
		ifdev->incoming_proto = &ipv4_protocol;

		printk_safe("%s: bringing interface up\n", ifdev->name);
		error = ifdev->ops->up(ifdev);
		if (error)
			return error;
	}
	else {
		printk_safe("%s: bringing interface down\n", ifdev->name);
		error = ifdev->ops->down(ifdev);
		if (error)
			return error;
	}

	ifdev->flags = state & IFF_SET_MASK;
	return 0;
}

int net_if_ioctl(int request, struct ifreq *ifreq, uid_t uid)
{
	struct if_device *ifdev;

	if (_IO_TYPE(request) != 'I' || _IO_PARAM(request) != sizeof(struct ifreq))
		return EINVAL;

	ifdev = net_if_find(ifreq->ifr_name, NULL);
	if (!ifdev)
		return ENODEV;

	switch (request) {
		case SIOCGIFFLAGS:
			ifreq->ifr_flags = ifdev->flags;
			break;
		case SIOCSIFFLAGS:
			if (uid != SU_UID)
				return EPERM;
			return net_if_change_state(ifdev, ifreq->ifr_flags);
		case SIOCGIFADDR:
			memcpy(&ifreq->ifr_addr, &ifdev->address, sizeof(struct sockaddr_storage));
			break;
		case SIOCSIFADDR:
			if (uid != SU_UID)
				return EPERM;
			memcpy(&ifdev->address, &ifreq->ifr_addr, sizeof(struct sockaddr_storage));
			break;
		default:
			return -1;
	}
	return 0;
}

int net_if_send_packet(struct if_device *ifdev, struct packet *pack)
{
	_queue_insert_after(&ifdev->tx_queue, &pack->node, ifdev->tx_queue.tail);
	ifdev->tx_stats.packets += 1;
	// TODO should you prevent calling this if currently processing the net bottom half
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

