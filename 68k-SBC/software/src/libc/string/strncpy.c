
#include <string.h>


char *strncpy(char *dest, const char *src, int n)
{
	char *ret = dest;

	for (; *src != '\0' && n > 0; src++, dest++, n--)
		*dest = *src;
	return ret;
}

