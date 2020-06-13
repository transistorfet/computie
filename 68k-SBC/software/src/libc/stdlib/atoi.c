
#include "stdio.h"


int atoi(const char *str)
{
	int num = 0, sign = 1;

	while (*str == ' ' || *str == '\n' || *str == '\t' || *str == '\r')
		str++;

	if (*str == '-') {
		sign = -1;
		str++;
	}

	for (; *str >= '0' && *str <= '9'; str++) {
		num *= 10;
		num += (*str - '0');
	}

	return num * sign;
}

