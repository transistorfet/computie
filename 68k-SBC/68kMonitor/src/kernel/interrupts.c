
#include "kernel/interrupts.h"


#define STACK_POINTER_INIT	0x101000

extern void _start();
extern void _error();

const interrupt_handler_t init_vectors[8] __attribute__((section(".vectors"))) = {
	(interrupt_handler_t) STACK_POINTER_INIT,
	_start,
	_error,
	_error,
	_error,
	_error,
	_error,
	_error,
};



