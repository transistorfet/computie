
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

struct block {
	int size;
	struct block *next;
};

struct heap {
	struct block *free_blocks;
};


static int last_increase = 1024;
static struct heap main_heap = { 0 };

void *malloc(int size)
{
	struct block *prev = NULL;
	struct block *nextfree = NULL;
	struct block *cur = main_heap.free_blocks;

	// Align the size to 4 bytes
	size += ((4 - (size & 0x3)) & 0x3);
	int block_size = size + sizeof(struct block);

retry:
	for (; cur; prev = cur, cur = cur->next) {
		if (cur->size >= block_size) {
			// If the block can be split with enough room for another block struct and more than 8 bytes left over, then split it
			if (cur->size >= block_size + sizeof(struct block) + 8) {
				nextfree = (struct block *) ((char *) cur + block_size);
				nextfree->size = cur->size - block_size;
				cur->size = block_size;

				nextfree->next = cur->next;

			} else {
				nextfree = cur->next;
			}
			cur->next = NULL;

			if (prev) {
				prev->next = nextfree;
			} else {
				main_heap.free_blocks = nextfree;
			}

			return (void *) (cur + 1);
		}
	}

	// Double the last increase (and if it's not big enough to allocate the requested size, keep doubling until it can)
	do {
		last_increase <<= 1;
	} while (last_increase < block_size);

	// Ask the kernel to increase the data segment
	cur = sbrk(0);
	if (!sbrk(last_increase)) {
		// Out Of Memory
		printf("Out of memory!\n");
		return NULL;
	}

	// Add the new block to the free list
	cur->size = last_increase;
	if (prev)
		prev->next = cur;
	else {
		cur->next = main_heap.free_blocks;
		main_heap.free_blocks = cur;
	}

	goto retry;
}

void free(void *ptr)
{
	struct block *prev = NULL;
	struct block *block = ((struct block *) ptr) - 1;

	for (struct block *cur = main_heap.free_blocks; cur; prev = cur, cur = cur->next) {
		if (cur->next == block) {
			printf("Double free detected at %x! Halting...\n", cur);
			return;
		}

		if ((struct block *) ((char *) cur + cur->size) == block) {
			// Merge the free'd block with the previous block
			cur->size += block->size;

			// If this block is adjacent to the next free block, then merge them
			if ((struct block *) ((char *) cur + cur->size) == cur->next) {
				cur->size += cur->next->size;
				cur->next = cur->next->next;
			}
			return;
		}

		if (cur >= block) {
			// Insert the free'd block into the list
			if (prev)
				prev->next = block;
			else
				main_heap.free_blocks = block;
			block->next = cur;

			// If this block is adjacent to the next free block, then merge them
			if ((struct block *) ((char *) block + block->size) == cur) {
				block->size += cur->size;
				block->next = cur->next;
			}
			return;
		}
	}
}

/*
void print_free()
{
	int i = 0;
	struct block *cur = main_heap.free_blocks;
	for (; cur; cur = cur->next, i++) {
		printf("%d %x %x\n", i, cur, cur->size);
	} 
}
*/
