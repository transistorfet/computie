
#include <stdio.h>

#define _STDIO_MAX_STRING	200

int vfprintf(FILE *stream, const char *fmt, va_list args)
{
	int r;
	char buffer[_STDIO_MAX_STRING];

	r = vsnprintf(buffer, _STDIO_MAX_STRING, fmt, args);
	fputs(buffer, stream);

	return r;
}

