
#include <time.h>
#include <string.h>
#include <stdlib.h>


extern char *itoa_padded(int num, char *buffer, int radix, char width, char zeropad, char is_signed);


static char *_weeknames[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char *_monthnames[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

size_t strftime (char* buffer, size_t max, const char *fmt, const struct tm *tp)
{
	int i = 0;
	int value;
	short width;
	char padding;
	char *svalue;
	char *subfmt;

	for (int j = 0; fmt[j] != '\0' && i < max; j++) {
		if (fmt[j] == '%') {
			svalue = NULL;
			subfmt = NULL;
			padding = '0';
			width = 2;
			switch (fmt[++j]) {
			    case 'a':
				svalue = _weeknames[tp->tm_wday];
				break;
			    case 'b':
			    case 'h':
				svalue = _monthnames[tp->tm_mon];
				break;
			    case 'c':
				subfmt = "%a %b %d %H:%M:%S %Y";
				break;
			    case 'd':
				value = tp->tm_mday;
				break;
			    case 'D':
				subfmt = "%m/%d/%y";
				break;
			    case 'e':
				value = tp->tm_mday;
				padding = ' ';
				break;
			    case 'F':
				subfmt = "%Y-%m-%d";
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
			    case 'p':
				svalue = tp->tm_hour < 12 ? "AM" : "PM";
				break;
			    case 'R':
				subfmt = "%H:%M";
				break;
			    case 'S':
				value = tp->tm_sec;
				break;
			    case 'T':
				subfmt = "%H:%M:%S";
				break;
			    case 'w':
				value = tp->tm_wday;
				break;
			    case 'W':
				value = tp->tm_yday / 7;
				break;
			    case 'y':
				value = tp->tm_year % 100;
				break;
			    case 'Y':
				value = tp->tm_year + 1900;
				width = 4;
				break;
			    default:
				// Set width to 0 to skip the itoa call
				width = 0;

				if (fmt[j] == 'n')
					value = '\n';
				else if (fmt[j] == 't')
					value = '\t';
				else
					value = fmt[j];
				buffer[i++] = value;
				break;
			}

			if (svalue) {
				strncpy(&buffer[i], svalue, max - i);
				i += strlen(svalue);
				if (i > max)
					i = max - 1;
			}
			else if (subfmt) {
				i += strftime(&buffer[i], max - i, subfmt, tp);
			}
			else if (width && width < max - i) {
				itoa_padded(value, &buffer[i], 10, width, padding, 0);
				i += width;
			}
		}
		else
			buffer[i++] = fmt[j];
	}
	buffer[i] = '\0';

	return i;
}

