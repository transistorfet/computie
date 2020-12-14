
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int strcmp(const char *, const char *);

int strlen(const char *);
int strnlen(const char *, int);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, int);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, int);
int strcnt(const char *, const char);
const char *strstr(const char *, const char *);
char *strchr(const char *, int);
char *strrchr(const char *, int);

void *memcpy_s(void *dest, const void *src, size_t num);	// NOTE it seems if this is called "memcpy", it will always be optimized away
void *memset_s(void *ptr, int value, size_t num);		// NOTE it seems if this is called "memset", it will always be optimized away
int memcmp(const void *ptr1, const void *ptr2, size_t num);

#endif
