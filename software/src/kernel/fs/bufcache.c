
#include <stddef.h>
#include <string.h>

#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>

#include "bufcache.h"
#include "../interrupts.h"
#include "../misc/queue.h"


#define BC_BLOCK_SIZE		1024
#define BC_ALLOC_BLOCK()	(kmalloc(BC_BLOCK_SIZE))
#define BC_FREE_BLOCK(ptr)	kmfree(ptr)


#define BLOCKCACHE_MAX		20

// TODO this is a hack until you have a driver interface
static char *device_base_tmp = (char *) 0x1E0000;

static struct queue cache;
static struct buf blocks[BLOCKCACHE_MAX];

static inline struct buf *_find_free_entry();
static struct buf *_load_block(device_t dev, block_t num);
static inline int _read_entry(struct buf *entry);
static inline int _write_entry(struct buf *entry);

void init_bufcache()
{
	_queue_init(&cache);

	for (short i = 0; i < BLOCKCACHE_MAX; i++) {
		_queue_node_init(&blocks[i].node);
		blocks[i].refcount = 0;
		blocks[i].flags = 0;
		blocks[i].block = NULL;
		_queue_insert(&cache, &blocks[i].node);
	}
}

void sync_bufcache()
{
	for (short i = 0; i < BLOCKCACHE_MAX; i++) {
		if (blocks[i].flags & BCF_ALLOCATED)
			_write_entry(&blocks[i]);
	}
}

struct buf *get_block(device_t dev, block_t num)
{
	struct buf *cur;

	for (cur = (struct buf *) cache.head; cur; cur = (struct buf *) cur->node.next) {
		if (cur->flags & BCF_ALLOCATED && cur->dev == dev && cur->num == num) {
			cur->refcount++;

			// Move to the front of the cache list
			_queue_remove(&cache, &cur->node);
			_queue_insert(&cache, &cur->node);
			return cur;
		}
	}

	return _load_block(dev, num);
}

int release_block(struct buf *buf, short dirty)
{
	if (dirty)
		mark_block_dirty(buf);

	if (--buf->refcount == 0) {
		// TODO we actually maybe don't want to write until this entry is recycled, or else any bit changes will require an immediate writeback
		//_write_entry(buf);
	}
	else if (buf->refcount < 0) {
		buf->refcount = 0;
		printk("Error: possible double free for block %d:%d\n", buf->dev, buf->num);
	}
}

void mark_block_dirty(struct buf *buf)
{
	buf->flags |= BCF_DIRTY;
}

static struct buf *_load_block(device_t dev, block_t num)
{
	struct buf *entry;

	entry = _find_free_entry();
	// TODO set errno?
	if (!entry)
		return NULL;

	_queue_insert(&cache, &entry->node);

	entry->refcount = 1;
	entry->flags = BCF_ALLOCATED;
	entry->dev = dev;
	entry->num = num;
	entry->block = BC_ALLOC_BLOCK();

	if (_read_entry(entry) < 0)
		return NULL;

	return entry;
}

static inline struct buf *_find_free_entry()
{
	// Recycle the last used entry
	struct buf *last = (struct buf *) cache.tail;

	if (!last || last->refcount > 0) {
		panic("Error: ran out of bufcache entries\n");
		return NULL;
	}

	_queue_remove(&cache, &last->node);
	if (last->block) {
		_write_entry(last);
		BC_FREE_BLOCK(last->block);
	}
	return last;
}


static inline int _read_entry(struct buf *entry)
{
	if (entry->num >= 128)
		return -1;
	printk("READING: %x <- %x x %x\n", entry->block, device_base_tmp + (entry->num * BC_BLOCK_SIZE), BC_BLOCK_SIZE);
	memcpy_s(entry->block, device_base_tmp + (entry->num * BC_BLOCK_SIZE), BC_BLOCK_SIZE);
	return 0;
}

static inline int _write_entry(struct buf *entry)
{
	if (!(entry->flags & BCF_DIRTY))
		return 0;
	printk("WRITING: %x <- %x x %x\n", device_base_tmp + (entry->num * BC_BLOCK_SIZE), entry->block, BC_BLOCK_SIZE);
	memcpy_s(device_base_tmp + (entry->num * BC_BLOCK_SIZE), entry->block, BC_BLOCK_SIZE);
	entry->flags &= ~BCF_DIRTY;
	return 1;
}
				
