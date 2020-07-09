
#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

struct vdir;
struct stat;

int fork();
void exit(int exitcode);
int exec(const char *path);
int open(const char *path, int flags);
int close(int fd);
size_t read(int fd, char *buf, size_t nbytes);
size_t write(int fd, const char *buf, size_t nbytes);
int readdir(int fd, struct vdir *dir);
int stat(const char *path, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);

#endif
