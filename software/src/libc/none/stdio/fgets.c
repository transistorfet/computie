
#include <stdio.h>
#include <string.h>

char *fgets(char *str, int max, FILE *stream)
{
	int i = 0;

	for (; i < max - 1; i++) {
		str[i] = getchar();
		if (str[i] == '\n') {
			i += 1;
			break;
		}
	}
	str[i] = '\0';
	return str;
}

