
#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include <stddef.h>

typedef struct _FILE {
	int fd;
} FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);

int snprintf(char *buffer, size_t n, const char *fmt, ...);
int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args);


int fgetc(FILE *stream);
int fgets(char *str, int num, FILE *stream);
int getchar(void);

int fputc(int ch, FILE *stream);
int fputs(const char *str, FILE *stream);
int putsn(const char *str);	// Non-standard, same as puts but without the extra newline
int puts(const char *str);
int putchar(int);

size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);

#endif
