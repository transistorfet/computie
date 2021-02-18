
#include <string.h>

static inline char _compare(const char *str1, const char *str2)
{
	for (;*str1 && *str2; str1++, str2++) {
		if (*str1 != *str2)
			return 0;
	}
	return 1;
}

char *strstr(const char *str1, const char *str2)
{
	for (; *str1; str1++) {
		if (_compare(str1, str2))
			return (char *) str1;
	}
	return NULL;
}

