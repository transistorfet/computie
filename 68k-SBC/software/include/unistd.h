
#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

struct vdir;

int fork();
void exit(int exitcode);
int exec(const char *path);
int open(const char *path, int flags);
int close(int fd);
size_t read(int fd, char *buf, size_t nbytes);
size_t write(int fd, const char *buf, size_t nbytes);
int readdir(int fd, struct vdir *dir);

#endif
