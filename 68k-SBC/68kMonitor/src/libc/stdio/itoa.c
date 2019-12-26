
#include "stdio.h"


char *itoa(int num, char *buffer, char radix)
{
	char c;
	int i = 0;
	int sign = 0;

	if (num < 0) {
		sign = 1;
		num *= -1;
	}

	do {
		buffer[i] = (num % radix) + '0';
		if (buffer[i] > '9')
			buffer[i] += 0x07;
		num /= radix;
		i++;
	} while (num > 0);

	if (sign)
		buffer[i++] = '-';
	buffer[i--] = '\0';

	for (int j = 0; j < i; j++, i--) {
		c = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = c;
	}
	return buffer;
}

