
#include <stdio.h>

#define _STDIO_MAX_INPUT	2048

int vsprintf(char *buffer, const char *fmt, va_list args)
{
	return vsnprintf(buffer, _STDIO_MAX_INPUT, fmt, args);
}

