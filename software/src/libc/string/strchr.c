
#include <string.h>

const char *strchr(const char *str, int ch) {
	for (; *str; str++) {
		if (*str == ch)
			return str;
	}
	return NULL;
}

