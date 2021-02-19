
#include <stdio.h>
#include <string.h>

int fgets(char *str, int max, FILE *stream)
{
	int i = 0;

	for (; i < max; i++) {
		str[i] = getchar();
		if (str[i] == '\n')
			break;
	}
	str[i] = '\0';
	return i;
}

