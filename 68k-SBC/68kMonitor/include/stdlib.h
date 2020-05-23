
#ifndef STDLIB_H
#define STDLIB_H

int atoi(const char *str);
char *itoa(int num, char *buffer, int radix);
long int strtol(const char *str, const char **endptr, int base);

#endif
