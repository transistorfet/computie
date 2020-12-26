
#include <string.h>

char *strcat(char *dest, const char *src)
{
	char *ret = dest;

	for (; *dest; dest++) { }

	for (; *src; src++, dest++)
		*dest = *src;
	*dest = '\0';

	return ret;
}


