
#include <string.h>
#include <stdint.h>


int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	register char data;

	for (; num > 0; num--) {
		data = *((volatile unsigned char *) ptr1++) - *((volatile unsigned char *) ptr2++);
		if (data != 0)
			return data < 0 ? -1 : 1;
	}
	return 0;
}

