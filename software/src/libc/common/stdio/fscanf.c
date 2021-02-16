
#include <stdio.h>

int fscanf(FILE *stream, const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vfscanf(stream, fmt, args);
	va_end(args);

	return r;
}

