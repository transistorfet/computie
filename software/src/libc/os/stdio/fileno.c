#include <stdio.h>
#include "file.h"

int fileno(FILE *stream)
{
	return stream->fd;
}

