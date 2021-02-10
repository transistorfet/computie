
#include <stdio.h>


long int strtol(const char *str, const char **endptr, int base)
{
	char b, sign = 1;
	int num = 0;

	while (*str == ' ' || *str == '\n' || *str == '\t' || *str == '\r')
		str++;

	if (*str == '-') {
		sign = -1;
		str++;
	}
	else if (*str == '0') {
		if (str[1] == 'x') {
			base = 16;
			str += 2;
		}
		else {
			base = 8;
			str++;
		}
	}


	for (; 1; str++) {
		if (*str >= '0' && *str <= '9')
			b = *str - '0';
		else if (*str >= 'A' && *str <= 'Z')
			b = *str - 'A' + 10;
		else if (*str >= 'a' && *str <= 'z')
			b = *str - 'a' + 10;
		else
			b = base;

		if (b >= base)
			break;

		num *= base;
		num += b;
	}


	if (endptr)
		*endptr = str;

	return num * sign;
}

