
#include <stddef.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>


struct block {
	int size;
	struct block *next;
};

struct heap {
	struct block *free_blocks;
};


static struct heap main_heap = { 0 };


void init_kernel_heap(void *addr, int size)
{
	struct block *space = (struct block *) addr;

	space->size = size;
	space->next = NULL;

	main_heap.free_blocks = space;
}

void *kmalloc(int size)
{
	struct block *prev = NULL;
	struct block *nextfree = NULL;
	struct block *cur = main_heap.free_blocks;

	// Align the size to 4 bytes
	size += ((4 - (size & 0x3)) & 0x3);
	int block_size = size + sizeof(struct block);

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
	// Out Of Memory
	panic("Kernel out of memory!  Halting...\n");
	return NULL;
}

void kmfree(void *ptr)
{
	struct block *prev = NULL;
	struct block *block = ((struct block *) ptr) - 1;

	for (struct block *cur = main_heap.free_blocks; cur; prev = cur, cur = cur->next) {
		if (cur->next == block) {
			panic("Double free detected at %x! Halting...\n", cur);
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
	printk_safe("free list:\n");
	for (struct block *cur = main_heap.free_blocks; cur; cur = cur->next) {
		printk_safe("%x: %x\n", cur, cur->size);
	}
}
*/

