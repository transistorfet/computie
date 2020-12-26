

#include <string.h>

int strncmp(const char *str1, const char *str2, int max)
{
	while (max) {
		if (*str1 > *str2)
			return 1;
		else if (*str1 < *str2)
			return -1;
		else if (*str1 == *str2 && *str1 == '\0')
			return 0;
		str1++;
		str2++;
		max--;
	}
	return 0;
}

