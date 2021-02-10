
#include <string.h>
#include <stdint.h>

void *memset_s(void *ptr, int value, size_t num)
{
	register unsigned char *dest = ptr;

	for (; num > 0; num--)
		*dest++ = (char) value;
	/*
	asm volatile(
	"move.l	%0, %%a0\n"
	"move.l	%1, %%d0\n"
	"_memset:\n"
	"move.b	%%d0, (%%a0)\n"
	"add.l	#1, %%a0\n"
	"sub.l	#1, %2\n"
	"bne	_memset\n"
	:
	: "g" (ptr), "g" (value), "g" (num)
	);
	*/
	return ptr;
}

extern void *memset(void *ptr, int value, size_t num) __attribute__((alias("memset_s")));
