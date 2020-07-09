
#include <string.h>


int strlen(const char *str)
{
	register int i = 0;

	for (; str[i] != '\0'; i++)
		/* intentionally empty */;
	return(i);
}

