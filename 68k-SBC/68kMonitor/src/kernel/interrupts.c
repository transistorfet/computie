
#include "kernel/interrupts.h"


#define STACK_POINTER_INIT	0x101000

extern void _start();
extern void _error();

const void *init_vectors[8] __attribute__((section(".vectors"))) = {
	(void *) STACK_POINTER_INIT,
	(void *) _start,
	(void *) _error,
	(void *) _error,
	(void *) _error,
	(void *) _error,
	(void *) _error,
	(void *) _error,
};



