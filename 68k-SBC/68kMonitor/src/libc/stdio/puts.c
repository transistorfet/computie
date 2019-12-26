
#include "tty.h"
#include "stdio.h"


int puts(const char *str)
{
	for (; *str != '\0'; str++)
		putchar(*str);
}

