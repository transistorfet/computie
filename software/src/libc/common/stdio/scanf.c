
#include <stdio.h>

int scanf(const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vfscanf(stdout, fmt, args);
	va_end(args);

	return r;
}

