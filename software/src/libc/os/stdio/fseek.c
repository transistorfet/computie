
#include <stdio.h>
#include <unistd.h>
#include "file.h"

int fseek(FILE *stream, long int offset, int origin)
{
	return lseek(stream->fd, offset, origin);
}

