
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int strlen(const char *);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, int);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, int);
const char *strchr(const char *, int);
const char *strrchr(const char *, int);
char *strcat(char *, const char *);

/*
int strcnt(const char *, const char);
const char *strstr(const char *, const char *);
*/

void *memcpy(void *dest, const void *src, size_t num);
void *memset(void *ptr, int value, size_t num);
int memcmp(const void *ptr1, const void *ptr2, size_t num);

void *memcpy_s(void *dest, const void *src, size_t num);	// NOTE it seems if this is called "memcpy", it will always be optimized away
void *memset_s(void *ptr, int value, size_t num);		// NOTE it seems if this is called "memset", it will always be optimized away

#endif
