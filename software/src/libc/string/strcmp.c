
#include <string.h>

int strcmp(const char *str1, const char *str2)
{
	while (1) {
		if (*str1 > *str2)
			return 1;
		else if (*str1 < *str2)
			return -1;
		else if (*str1 == *str2 && *str1 == '\0')
			return 0;
		str1++;
		str2++;
	}
}

