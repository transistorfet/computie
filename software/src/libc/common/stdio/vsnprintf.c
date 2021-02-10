
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern char *itoa_padded(int num, char *buffer, int radix, char width, char zeropad);


int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args)
{
	int len;
	int i = 0;

	for (int j = 0; i < n - 1 && fmt[j] != '\0'; j++) {
		if (fmt[j] == '%') {
			char type;
			char width = 0;
			char zeropad = 0;

			// Should it be zero padding or spaces
			if (fmt[j + 1] == '0') {
				zeropad = 1;
				j += 1;
			}

			// Parse the width to pad the data to
			if (fmt[j + 1] >= '0' && fmt[j + 1] <= '9') {
				char *endptr;
				width = strtol(&fmt[j + 1], &endptr, 10);
				j += endptr - &fmt[j] - 1;
			}

			// TODO parse out the precision

			// Parse the data type
			type = fmt[++j];

			// Fetch the data and format it accordingly
			switch (type) {
			    case 's': {
				const char *s;

				s = va_arg(args, const char *);
				len = strlen(s);
				strncpy(&buffer[i], s, n - i - 1);
				i += len;
				break;
			    }
			    case 'd':
			    case 'x':
			    case 'X': {
				int d;

				d = va_arg(args, int);
				itoa_padded(d, &buffer[i], type == 'x' ? 16 : 10, width, zeropad);
				i += strlen(&buffer[i]);
				break;
			    }
			    case 'c': {
				int c;

				c = va_arg(args, int);
				buffer[i++] = (char) c;
				break;
			    }
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

