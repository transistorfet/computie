
#include <stdio.h>

int vscanf(const char *fmt, va_list args)
{
	return vfscanf(stdout, fmt, args);
}

