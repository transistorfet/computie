
#include <stdio.h>
#include <unistd.h>
#include "file.h"

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	size = count == 1 ? size : size * count;
	return write(stream->fd, ptr, size);
}

