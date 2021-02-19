
#include <stdio.h>
#include <unistd.h>
#include "file.h"

int fseek(FILE *stream, long int offset, int origin)
{
	stream->count = 0;
	return lseek(stream->fd, offset, origin);
}

