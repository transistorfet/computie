
#include <fcntl.h>
#include <string.h>

#include <kernel/bh.h>
#include <kernel/driver.h>
#include <kernel/printk.h>

#include "../if.h"
#include "../packet.h"
#include "../protocol.h"


#define SLIP_DEVICES		1
#define SLIP_MTU_MAX		1500

#define SLIP_FRAME_END		0xC0
#define SLIP_FRAME_ESC		0xDB
#define SLIP_FRAME_ESC_END	0xDC
#define SLIP_FRAME_ESC_ESC	0xDD

#define SLIP_IFDEV(x)		((struct slip_if_device *) (x))

int slip_if_init();
int slip_if_up(struct if_device *ifdev);
int slip_if_down(struct if_device *ifdev);
int slip_if_poll(struct if_device *ifdev);


struct if_ops slip_if_ops = {
	slip_if_init,
	slip_if_up,
	slip_if_down,
	slip_if_poll,
};

struct slip_if_device {
	struct if_device ifdev;

	device_t rdev;

	short rx_read;
	short rx_write;
	unsigned char rx_buffer[SLIP_MTU_MAX];

	short tx_read;
	short tx_write;
	unsigned char tx_buffer[SLIP_MTU_MAX];
};

static struct slip_if_device devices[SLIP_DEVICES];

static void slip_if_process_input(void *_unused);
static void slip_if_write_data(struct slip_if_device *ifdev);
static void slip_if_read_data(struct slip_if_device *ifdev);

int slip_if_init()
{
	memset(devices, '\0', sizeof(struct slip_if_device) * SLIP_DEVICES);
	devices[0].ifdev.ops = &slip_if_ops;
	devices[0].ifdev.name = "slip0";
	devices[0].ifdev.mtu = SLIP_MTU_MAX;
	devices[0].rdev = DEVNUM(DEVMAJOR_TTY68681, 1);

	register_bh(BH_SLIP, slip_if_process_input, NULL);
	for (short i = 0; i < SLIP_DEVICES; i++)
		net_if_register_device((struct if_device *) &devices[i]);
}

int slip_if_up(struct if_device *ifdev)
{
	// TODO you need to notify the serial driver to request the net bottom half run after any input
	// TODO should you have a mode flag to exclusively open a device, which gives an error if it's already open
	return dev_open(SLIP_IFDEV(ifdev)->rdev, O_RDWR | O_NONBLOCK);
}

int slip_if_down(struct if_device *ifdev)
{
	return dev_close(SLIP_IFDEV(ifdev)->rdev);
}

int slip_if_poll(struct if_device *ifdev)
{
	slip_if_write_data((struct slip_if_device *) ifdev);
	slip_if_read_data((struct slip_if_device *) ifdev);
	return 0;
}

static void slip_encode_packet(struct slip_if_device *ifdev)
{
	int j = 0;
	struct packet *pack;

	// NOTE we assume the tx buffer is empty when this is called

	if (!ifdev->ifdev.tx_queue.head)
		return;

	pack = (struct packet *) ifdev->ifdev.tx_queue.head;
	_queue_remove(&ifdev->ifdev.tx_queue, &pack->node);


	if (ifdev->ifdev.flags & IFF_DEBUG) {
		printk_safe("DEBUG: send packet: ");
		for (int i = 0; i < pack->length; i++) {
			printk_safe("%x ", (int) pack->data[i]);
		}
		printk_safe("\n");
	}


	ifdev->tx_buffer[j++] = SLIP_FRAME_END;
	for (short i = 0; i < pack->length; i++) {
		switch (pack->data[i]) {
			case SLIP_FRAME_END:
				ifdev->tx_buffer[j++] = SLIP_FRAME_ESC;
				ifdev->tx_buffer[j++] = SLIP_FRAME_ESC_END;
				break;
			case SLIP_FRAME_ESC:
				ifdev->tx_buffer[j++] = SLIP_FRAME_ESC;
				ifdev->tx_buffer[j++] = SLIP_FRAME_ESC_ESC;
				break;
			default:
				ifdev->tx_buffer[j++] = pack->data[i];
				break;
		}
	}
	ifdev->tx_buffer[j++] = SLIP_FRAME_END;
	ifdev->tx_read = 0;
	ifdev->tx_write = j;

	packet_free(pack);
}

static void slip_decode_packet(struct slip_if_device *ifdev, short length)
{
	short i, j;
	struct packet *pack;

	pack = packet_alloc(&ifdev->ifdev, ifdev->ifdev.incoming_proto, length);
	if (!pack) {
		// TODO what do you do if there's an error here
		return;
	}

	for (i = 0, j = 0; i < length; i++, j++) {
		if (ifdev->rx_buffer[i] == SLIP_FRAME_END)
			break;
		else if (ifdev->rx_buffer[i] == SLIP_FRAME_ESC) {
			i += 1;
			if (ifdev->rx_buffer[i] == SLIP_FRAME_ESC_END)
				pack->data[j] = SLIP_FRAME_END;
			else if (ifdev->rx_buffer[i] == SLIP_FRAME_ESC_ESC)
				pack->data[j] = SLIP_FRAME_ESC;
		}
		else
			pack->data[j] = ifdev->rx_buffer[i];
	}
	pack->length = j;

	if (ifdev->ifdev.flags & IFF_DEBUG) {
		printk_safe("DEBUG: recv packet: ");
		for (int i = 0; i < pack->length; i++)
			printk_safe("%x ", (int) pack->data[i]);
		printk_safe("\n");
	}

	_queue_insert_after(&ifdev->ifdev.rx_queue, &pack->node, ifdev->ifdev.rx_queue.tail);
	request_bh_run(BH_NET);
}

static void slip_if_write_data(struct slip_if_device *ifdev)
{
	int written;

	do {
		written = 0;

		if (!ifdev->tx_write)
			slip_encode_packet(ifdev);

		if (ifdev->tx_read < ifdev->tx_write) {
			written = dev_write(ifdev->rdev, &ifdev->tx_buffer[ifdev->tx_read], 0, ifdev->tx_write - ifdev->tx_read);
			if (written < 0) {
				//devices[i].error = read;
				return;
			}
			ifdev->tx_read += written;

			if (ifdev->tx_read >= ifdev->tx_write) {
				ifdev->tx_read = 0;
				ifdev->tx_write = 0;
			}
		}
	} while (written);
}

static void slip_if_read_data(struct slip_if_device *ifdev)
{
	int read;

	read = dev_read(ifdev->rdev, &ifdev->rx_buffer[ifdev->rx_write], 0, SLIP_MTU_MAX - ifdev->rx_write);
	if (read < 0) {
		//devices[i].error = read;
		return;
	}
	ifdev->rx_write += read;

	//for (int i = 0; i < ifdev->rx_write; i++)
	//	printk_safe("%x ", (int) ifdev->rx_buffer[i]);
	//printk_safe("\n");

	for (; read; read--) {
		if (ifdev->rx_buffer[ifdev->rx_read] == SLIP_FRAME_END) {
			if (ifdev->rx_read)
				slip_decode_packet(ifdev, ifdev->rx_read);
			if (++ifdev->rx_read < ifdev->rx_write)
				memcpy(ifdev->rx_buffer, &ifdev->rx_buffer[ifdev->rx_read], ifdev->rx_write - ifdev->rx_read);
			ifdev->rx_write -= ifdev->rx_read;
			ifdev->rx_read = 0;
		}
		else
			ifdev->rx_read++;
	}

	ifdev->rx_read = ifdev->rx_write;
}

static void slip_if_process_input(void *_unused)
{
	for (short i = 0; i < SLIP_DEVICES; i++) {
		slip_if_read_data(&devices[i]);
		slip_if_write_data(&devices[i]);
	}
}


