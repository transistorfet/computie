
#include <stddef.h>

struct block {
	int size;
	struct block *next;
};

struct heap {
	struct block *free_blocks;
};


static struct heap main_heap = { 0 };


void init_heap(void *addr, int size)
{
	struct block *space = (struct block *) addr;

	space->size = size - sizeof(struct block);
	space->next = NULL;

	main_heap.free_blocks = space;
}

void *malloc(int size)
{
	struct block *prev = NULL;
	struct block *nextfree = NULL;
	struct block *cur = main_heap.free_blocks;

	// Align the size to 4 bytes
	size = size + ((size & 0x3) ? 4 : 0);
	int block_size = size + sizeof(struct block);

	for (; cur; prev = cur, cur = cur->next) {
		if (cur->size >= size) {
			// If the block can be split with >4 bytes unallocated, then split it
			if (cur->size >= block_size + 4) {
				nextfree = (struct block *) ((char *) cur + block_size);
				nextfree->size = cur->size - block_size;
				cur->size = size;

				nextfree->next = cur->next;

			} else {
				nextfree = cur->next;
			}

			if (prev) {
				prev->next = nextfree;
			} else {
				main_heap.free_blocks = nextfree;
			}

			return (void *) (cur + 1);
		}
	}
	// Out Of Memory
	return NULL;
}

void free(void *ptr)
{
	struct block *cur = ((struct block *) ptr) - 1;

	// TODO this doesn't keep the blocks in address order

	// Insert into free list
	cur->next = main_heap.free_blocks;
	main_heap.free_blocks = cur;
}

/*
#include <stdio.h>

void print_free()
{
	int i = 0;
	struct block *cur = main_heap.free_blocks;
	for (; cur; cur = cur->next, i++) {
		printf("%d %x %x\n", i, cur, cur->size);
	} 
}
*/
