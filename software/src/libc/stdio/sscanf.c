
#include <stdio.h>

int sscanf(const char *str, const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vsscanf(str, fmt, args);
	va_end(args);

	return r;
}

