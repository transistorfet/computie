
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int strlen(const char *str);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int max);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int max);
const char *strchr(const char *str, int ch);
const char *strrchr(const char *str, int ch);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, int max);

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
