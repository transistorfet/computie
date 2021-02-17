
#include <stdio.h>
#include "file.h"

int feof(FILE *stream)
{
	return stream->flags & _STDIO_FF_EOF;
}

