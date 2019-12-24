
#include "tty.h"
#include "stdio.h"
#include "string.h"


int puts(const char *str)
{
	for (; *str != '\0'; str++)
		putchar(*str);
}

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

#define _STDIO_MAX_STRING	200

int printf(const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vprintf(fmt, args);
	va_end(args);

	return r;
}

int vprintf(const char *fmt, va_list args)
{
	int r;
	char buffer[_STDIO_MAX_STRING];

	r = vsnprintf(buffer, _STDIO_MAX_STRING, fmt, args);
	puts(buffer);

	return r;
}


int snprintf(char *buffer, size_t n, const char *fmt, ...)
{
	int r;
	va_list args;

	va_start(args, fmt);
	r = vsnprintf(buffer, n, fmt, args);
	va_end(args);

	return r;
}

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
				i += len;
				break;
			    case 'd':
			    case 'x':
				d = va_arg(args, int);
				itoa(d, &buffer[i], fmt[j] == 'x' ? 16 : 10);
				i += strlen(&buffer[i]);
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


#ifdef __m68k__
int __divsi3(int a, int b)
{
	int c;

	asm(
	"move.w	%1, %%d0\n"
	"move.w	%2, %%d1\n"
	"divs	%%d1, %%d0\n"
	"move.w	%%d0, %0\n"
	: "=r" (c)
	: "r" (a), "r" (b)
	: "%d0", "%d1"
	);
	return c;
}

int __modsi3(int a, int b)
{
	int c;

	asm(
	"move.w	%1, %%d0\n"
	"move.w	%2, %%d1\n"
	"divs	%%d1, %%d0\n"
	"swap	%%d0\n"
	"move.w	%%d0, %0\n"
	: "=r" (c)
	: "r" (a), "r" (b)
	: "%d0", "%d1"
	);
	return c;
}
#endif


