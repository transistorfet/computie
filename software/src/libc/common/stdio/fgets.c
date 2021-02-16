
#include <stdio.h>
#include <string.h>

int fgets(char *str, int max, FILE *stream)
{
	int r = fread(str, max - 1, 1, stream);
	str[r] = '\0';
	return r;
}


