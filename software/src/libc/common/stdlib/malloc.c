
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
	size += ((4 - (size & 0x3)) & 0x3);
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
			cur->next = NULL;

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
	struct block *block = ((struct block *) ptr) - 1;

	/*
	for (struct block *cur = main_heap.free_blocks; cur; cur = cur->next) {
		if (cur == block) {
			// TODO this is only defined inside the kernel
			panic("Double free detected at %x! Halting...\n", cur);
		}
	}
	*/

	// TODO this doesn't keep the blocks in address order

	// Insert into free list
	block->next = main_heap.free_blocks;
	main_heap.free_blocks = block;
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
