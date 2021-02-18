
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern char *itoa_padded(int num, char *buffer, int radix, char width, char zeropad, char is_signed);


int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args)
{
	int len;
	int i = 0;

	for (int j = 0; i < n - 1 && fmt[j] != '\0'; j++) {
		if (fmt[j] == '%') {
			char type;
			char width = 0;
			char flags = 0;
			char length = 4;
			char zeropad = 0;
			char numprefix = 0;

			// Should it be zero padding or spaces
			if (fmt[j + 1] == '0') {
				zeropad = 1;
				j += 1;
			}
			else if (fmt[j + 1] == '#') {
				numprefix = 1;
				j += 1;
			}

			// Parse the width to pad the data to
			if (isdigit(fmt[j + 1])) {
				char *endptr;
				width = strtol(&fmt[j + 1], &endptr, 10);
				j += endptr - &fmt[j] - 1;
			}

			// TODO parse out the precision

			// Parse out length of data (for numbers mostly)
			if (fmt[j + 1] == 'l' || fmt[j + 1] == 'h') {
				if (fmt[j + 1] == 'h')
					length = 2;
				j += 1;
			}

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
			    case 'i':
				type = 'd';
			    case 'p':
			    case 'd':
			    case 'u':
			    case 'o':
			    case 'x':
			    case 'X': {
				// Extract the data from args based on the requested length
				unsigned int d;
				d = va_arg(args, unsigned int);

				// Determine what base to display the number in
				int radix = 10;
				if (type == 'x' || type == 'X' || type == 'p') {
					if (numprefix && d) {
						buffer[i++] = '0';
						buffer[i++] = 'x';
					}
					radix = 16;
				}
				else if (type == 'o') {
					if (numprefix && d)
						buffer[i++] = '0';
					radix = 8;
				}

				itoa_padded(d, &buffer[i], radix, width, zeropad, type == 'd' ? 1 : 0);
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

