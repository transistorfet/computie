
#include <time.h>

size_t strftime (char* buffer, size_t max, const char *fmt, const struct tm *tp)
{
	int i = 0;
	int value;
	short width;
	char padding;

	for (int j = 0; fmt[j] != '\0' && i < max; j++) {
		if (fmt[j] == '%') {
			padding = '0';
			width = 2;
			switch (fmt[++j]) {
			    case 'd':
				value = tp->tm_mday;
				break;
			    case 'e':
				value = tp->tm_mday;
				padding = ' ';
				break;
			    case 'H':
				value = tp->tm_hour;
				break;
			    case 'I':
				value = tp->tm_hour / 2;
				break;
			    case 'j':
				value = tp->tm_yday;
				width = 3;
				break;
			    case 'm':
				value = tp->tm_mon + 1;
				break;
			    case 'M':
				value = tp->tm_min;
				break;
			    case 'S':
				value = tp->tm_sec;
				break;
			    case 'w':
				value = tp->tm_wday;
				break;
			    case 'W':
				value = tp->tm_yday / 7;
				break;
			    case 'y':
				value = tp->tm_year;
				break;
			    case 'Y':
				value = tp->tm_year + 1900;
				width = 4;
				break;
			    case '%':
			    default:
				buffer[i++] = fmt[j];
				// Set width to 0 to skip the itoa call
				width = 0;
				break;
			}

			if (width && i + width < max) {
				itoa_padded(value, &buffer[i], 10, width, padding);
				i += width;
			}
		}
		else
			buffer[i++] = fmt[j];
	}
	buffer[i] = '\0';

	return i;
}

