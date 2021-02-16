
#include <stdio.h>

int printf(const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vfprintf(stdout, fmt, args);
	va_end(args);

	return r;
}

