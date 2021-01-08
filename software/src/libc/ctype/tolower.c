
#include <ctype.h>

int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return c + 0x20;
	return c;
}

