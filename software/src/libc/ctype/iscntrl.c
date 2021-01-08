
#include <ctype.h>

int iscntrl(int c)
{
	return (c >= 0x00 && c <= 0x1F);
}

