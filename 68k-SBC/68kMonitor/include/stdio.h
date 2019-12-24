
#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include "stddef.h"


int puts(const char *str);
char *itoa(int num, char *buffer, char radix);

int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);

int snprintf(char *buffer, size_t n, const char *fmt, ...);
int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args);

#endif
