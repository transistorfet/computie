
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args)
{
	int i = 0;
	int len;

	int d;
	const char *s;

	for (int j = 0; i < n - 1 && fmt[j] != '\0'; j++) {
		if (fmt[j] == '%') {
			switch (fmt[++j]) {
			    case 's':
				s = va_arg(args, const char *);
				len = strlen(s);
				strncpy(&buffer[i], s, n - i - 1);
				i += len - 1;
				break;
			    case 'd':
			    case 'x':
				d = va_arg(args, int);
				itoa(d, &buffer[i], fmt[j] == 'x' ? 16 : 10);
				i += strlen(&buffer[i]) - 1;
				break;
			    case 'c':
				d = va_arg(args, int);
				buffer[i++] = (char) d;
				break;
			    default:
				break;
			}
		}
		else {
			buffer[i++] = fmt[j];
		}
	}
	buffer[i] = '\0';

	return i;
}

