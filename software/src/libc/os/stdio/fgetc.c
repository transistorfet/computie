
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

int fgetc(FILE *stream)
{
	if (stream->count <= 0) {
		// If the stream is unbuffered, then do a direct read and return
		if (stream->flags & _IONBF) {
			unsigned char ch;
			if (fread(&ch, 1, 1, stream) > 0)
				return ch;
			return EOF;
		}

		// If no buffer is allocated, then allocate one
		if (!stream->buf) {
			stream->bufsize = BUFSIZE;
			stream->flags |= _IOFREEBUF;
			stream->buf = malloc(BUFSIZE);
		}

		stream->pos = 0;
		stream->count = fread(stream->buf, stream->bufsize, 1, stream);
		if (stream->count <= 0) {
			stream->count = 0;
			return EOF;
		}
	}

	stream->count -= 1;
	return (int) stream->buf[stream->pos++];
}

