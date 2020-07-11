
#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include <stddef.h>

// WHENCE argument to fseek
#define SEEK_SET	0	// Seek relative to the beginning of file
#define SEEK_CUR	1	// Seek relative to the current position
#define SEEK_END	2	// Seek relative to the end of file


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
