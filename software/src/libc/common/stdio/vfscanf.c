
#include <stdio.h>

#define _STDIO_MAX_STRING	200

int vfscanf(FILE *stream, const char *fmt, va_list args)
{
	int r;
	char buffer[_STDIO_MAX_STRING];

	fgets(buffer, _STDIO_MAX_STRING, stream);
	return vsscanf(buffer, fmt, args);
}

