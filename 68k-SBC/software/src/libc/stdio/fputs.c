
#include <stdio.h>
#include <string.h>


int fputs(const char *str, FILE *stream)
{
	fwrite(str, strlen(str), 1, stream);
}


