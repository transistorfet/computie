
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file.h"

int fclose(FILE *stream)
{
	int error;

	if ((stream->flags & _IOFREEBUF) && stream->buf)
		free(stream->buf);
	error = close(stream->fd);
	return error;
}

