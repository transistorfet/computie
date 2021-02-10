
#include <ctype.h>

int toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return c - 0x20;
	return c;
}

