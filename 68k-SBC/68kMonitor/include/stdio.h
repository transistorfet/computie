
#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include <stddef.h>

typedef int FILE;

int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);

int snprintf(char *buffer, size_t n, const char *fmt, ...);
int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args);

int fputs(const char *str, FILE *stream);
int putsn(const char *str);	// Non-standard, same as puts but without the extra newline
int puts(const char *str);

int getchar(void);
int putchar(int);

#endif
