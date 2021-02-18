
#include <string.h>

static inline char _search(const char *str2, char ch)
{
	for (size_t j = 0; str2[j]; j++) {
		if (str2[j] == ch)
			return 1;
	}
	return 0;
}

size_t strspn(const char *str1, const char *str2)
{
	size_t i;

	for (i = 0; str1[i]; i++) {
		if (!_search(str2, str1[i]))
			break;
	}
	return i;
}

