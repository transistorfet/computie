
#ifndef STRING_H
#define STRING_H

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


#endif
