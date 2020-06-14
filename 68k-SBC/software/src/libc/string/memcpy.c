
#include <string.h>
#include <stdint.h>


void *memcpy(void *dest, const void *src, size_t num)
{
	volatile unsigned char *vdest = dest;

	for (; num > 0; num--)
		*vdest++ = *((unsigned char *) src++);
}

