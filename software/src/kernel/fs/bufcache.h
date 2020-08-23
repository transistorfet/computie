
#ifndef _SRC_KERNEL_FS_BLOCKCACHE_H
#define _SRC_KERNEL_FS_BLOCKCACHE_H

#include <stddef.h>
#include <sys/types.h>

#include "../misc/queue.h"

#define BCF_ALLOCATED	0x01
#define BCF_DIRTY	0x02

struct buf {
	struct queue_node node;
	int refcount;
	short flags;
	device_t dev;
	block_t num;
	void *block;
};

void init_bufcache();
void sync_bufcache();
struct buf *get_block(device_t dev, block_t num);
int release_block(struct buf *buf, short dirty);
void mark_block_dirty(struct buf *buf);

#endif
