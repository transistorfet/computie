
#ifndef STDLIB_H
#define STDLIB_H

int atoi(const char *str);
char *itoa(int num, char *buffer, int radix);
long int strtol(const char *str, char **endptr, int base);

void init_heap(void *addr, int size);

void *malloc(int size);
void free(void *ptr);

#endif
