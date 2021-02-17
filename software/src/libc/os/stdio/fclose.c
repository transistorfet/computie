
#include <stdio.h>
#include <unistd.h>
#include "file.h"

int fclose(FILE *stream)
{
	int error;

	error = close(stream->fd);
	return error;
}

