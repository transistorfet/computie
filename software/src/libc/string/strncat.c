
#include <string.h>

char *strncat(char *dest, const char *src, int max)
{
	char *ret = dest;

	for (; *dest; dest++) { }

	for (; *src && max > 0; src++, dest++, max--)
		*dest = *src;
	if (max > 0)
		*dest = '\0';

	return ret;
}


