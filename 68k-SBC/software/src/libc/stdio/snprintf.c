
#include <stdio.h>


int snprintf(char *buffer, size_t n, const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vsnprintf(buffer, n, fmt, args);
	va_end(args);

	return r;
}

