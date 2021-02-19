
#include <stdio.h>
#include <unistd.h>
#include "file.h"

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	size_t result;

	size = count == 1 ? size : size * count;
	result = read(stream->fd, ptr, size);
	if (result == 0)
		stream->flags |= _IOEOF;
	if (result < 0)
		stream->flags |= _IOERR;
	return result;
}

