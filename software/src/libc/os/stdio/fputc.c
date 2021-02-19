
#include <stdio.h>
#include "file.h"
#include <unistd.h>

int fputc(int ch, FILE *stream)
{
	// TODO this is unbuffered atm
	unsigned char c = ch;
	write(stream->fd, &c, 1);
}

