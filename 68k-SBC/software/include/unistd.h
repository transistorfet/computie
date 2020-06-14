
#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

size_t read(int fd, char *buf, size_t nbytes);
size_t write(int fd, const char *buf, size_t nbytes);

#endif
