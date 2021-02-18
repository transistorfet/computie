
#include <stdio.h>

char *itoa_padded(int num, char *buffer, int radix, char width, char zeropad, char is_signed);


char *itoa(int num, char *buffer, int radix)
{
	return itoa_padded(num, buffer, radix, 0, 0, radix == 10 ? 1 : 0);
}

char *itoa_padded(int num, char *buffer, int radix, char width, char zeropad, char is_signed)
{
	char c;
	int i = 0;
	int j = 0;
	int sign = 0;

	if (is_signed && num < 0) {
		sign = 1;
		num = -num;
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

	for (; i < width; i++)
		buffer[i] = zeropad ? '0' : ' ';

	buffer[i--] = '\0';

	for (; j < i; j++, i--) {
		c = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = c;
	}
	return buffer;
}

