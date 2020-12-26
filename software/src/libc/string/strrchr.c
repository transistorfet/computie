
#include <string.h>

const char *strrchr(const char *str, int ch)
{
	char *pos = NULL;

	for (; *str; str++) {
		if (*str == ch)
			pos = str;
	}
	return pos;
}


