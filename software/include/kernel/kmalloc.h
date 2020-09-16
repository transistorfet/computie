
#ifndef INCLUDE_KERNEL_KMALLOC_H
#define INCLUDE_KERNEL_KMALLOC_H

void init_kernel_heap(void *addr, int size);
void *kmalloc(int size);
void kmfree(void *ptr);

#endif
