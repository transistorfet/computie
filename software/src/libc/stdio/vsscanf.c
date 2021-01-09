
#include <ctype.h>
#include <stdio.h>

int vsscanf(const char *str, const char *fmt, va_list args)
{
	int i = 0;
	int count = 0;

	for (int j = 0; fmt[j] != '\0' && str[i] != '\0'; j++) {
		if (fmt[j] == '%') {
			char type;
			char width = 0;
			char ignore = 0;

			// Should it be ignored or saved
			if (fmt[j + 1] == '*') {
				ignore = 1;
				j += 1;
			}

			// Parse the width to read
			if (isdigit(fmt[j + 1])) {
				char *endptr;
				width = strtol(&fmt[j + 1], &endptr, 10);
				j += endptr - &fmt[j] - 1;
			}

			// Parse the data type
			type = fmt[++j];

			switch (type) {
			    case 'i':
			    case 'd':
			    case 'x':
			    case 'X': {
				char *endptr;
				register int number = strtol(&str[i], &endptr, type == 'd' || type == 'i' ? 10 : 16);
				if (!ignore) {
					int *ptr = va_arg(args, int *);
					*ptr = number;
				}
				count++;
				i += (endptr - &str[i]);
				break;
			    }
			    case 's': {
				if (ignore)
					for (; !isspace(str[i]); i++) { }
				else {
					char *buffer = va_arg(args, char *);
					for (; !isspace(str[i]); i++)
						*buffer++ = str[i];
					*buffer = '\0';
				}
				break;
			    }
			    case 'c': {
				if (!width)
					width = 1;

				if (ignore)
					i += width;
				else {
					char *buffer = va_arg(args, char *);
					for (; width; width--, i++)
						*buffer++ = str[i];
				}
				break;
			    }
			    default: {
				// TODO what to do if the format string is incorrect
				break;
			    }
			}
		}
		else if (isspace(fmt[j])) {
			// Read any amount of whitespace from the input string
			for (; isspace(str[i]); i++) { }
		}
		else if (fmt[j] != str[i++])
			// For any other non-matching character, then fail and return
			break;
	}
	return count;
}

