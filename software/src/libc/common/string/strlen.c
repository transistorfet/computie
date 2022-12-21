
#include <string.h>


int strlen(const char *str)
{
	volatile register int i = 0;

	for (; str[i] != '\0'; i++) {
		/* intentionally empty */
	}
	return(i);
}

