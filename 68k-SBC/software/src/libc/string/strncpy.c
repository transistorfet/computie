
#include <string.h>


char *strncpy(char *dest, const char *src, int n)
{
	char *ret = dest;

	for (; n > 0; n--)
		*dest++ = *src != '\0' ? *src++ : 0;
	return ret;
}

