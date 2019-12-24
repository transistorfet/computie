
#ifndef STRING_H
#define STRING_H

#define NULL	0

int strcmp(const char *, const char *);

int strlen(const char *);
int strnlen(const char *, int);
int strcpy(char *, const char *);
int strncpy(char *, const char *, int);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, int);
int strcnt(const char *, const char);
const char *strstr(const char *, const char *);
char *strchr(const char *, int);
char *strrchr(const char *, int);


#endif
