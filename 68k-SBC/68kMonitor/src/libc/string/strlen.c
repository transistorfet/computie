
#include "string.h"


int strlen(const char *str)
{
	int i = 0;

	while (str[i++] != '\0')
		/* intentionally empty */;
	return(i);
}

