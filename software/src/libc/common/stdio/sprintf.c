
#include <stdio.h>

int sprintf(char *buffer, const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vsprintf(buffer, fmt, args);
	va_end(args);

	return r;
}

