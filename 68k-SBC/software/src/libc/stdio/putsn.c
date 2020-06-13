
#include <stdio.h>


int putsn(const char *str)
{
	for (; *str != '\0'; str++)
		putchar(*str);
}

