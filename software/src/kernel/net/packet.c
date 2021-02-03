
#include <string.h>
#include <kernel/kmalloc.h>

#include "if.h"
#include "packet.h"
#include "../misc/queue.h"


struct packet *packet_alloc(struct if_device *ifdev, size_t capacity)
{
	struct packet *pack;

	pack = kmalloc(sizeof(struct packet) + capacity);
	if (!pack)
		return NULL;
	memset(pack, '\0', sizeof(struct packet) + capacity);

	_queue_node_init(&pack->node);
	pack->sock = NULL;
	pack->ifdev = ifdev;
	pack->capacity = capacity;

	return pack;
}

void packet_free(struct packet *pack)
{
	// TODO should you search through the linked list and free all??
	// TODO if headers are allocated separately, then free them
	kmfree(pack);
}

int packet_append(struct packet *pack, const void *ptr, int nbytes)
{
	if (pack->length + nbytes > pack->capacity)
		return -1;
	memcpy(&pack->data[pack->length], ptr, nbytes);
	pack->length += nbytes;
	return 0;
}

