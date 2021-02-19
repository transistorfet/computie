
#include <stdio.h>
#include <string.h>

char *fgets(char *str, int max, FILE *stream)
{
	int i;

	for (i = 0; i < max - 1; i++) {
		str[i] = fgetc(stream);
		if (str[i] == '\n') {
			i += 1;
			break;
		}
		else if (str[i] == EOF) {
			if (!i)
				return NULL;
			break;
		}
	}
	str[i] = '\0';
	return str;
}

