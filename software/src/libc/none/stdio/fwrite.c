
#include <stdio.h>
#include <unistd.h>


size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	size = count == 1 ? size : size * count;
	for (int i = size; i; i--, ptr++)
		putchar(*((char *) ptr));
	return size;
}

