
#include <stdio.h>
#include <string.h>

int fputs(const char *str, FILE *stream)
{
	for (; *str != '\0'; str++)
		putchar(*str);
}

