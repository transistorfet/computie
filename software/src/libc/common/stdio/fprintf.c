
#include <stdio.h>

int fprintf(FILE *stream, const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vfprintf(stream, fmt, args);
	va_end(args);

	return r;
}

