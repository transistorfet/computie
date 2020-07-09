
#ifndef _SRC_KERNEL_API_H
#define _SRC_KERNEL_API_H

#include <sys/stat.h>

extern void init_syscall();
void do_syscall();

extern void do_exit(int exitcode);
extern int do_fork();
extern size_t do_read(int fd, char *buf, size_t nbytes);
extern size_t do_write(int fd, const char *buf, size_t nbytes);
extern int do_open(const char *path, int oflags);
extern int do_close(int fd);
extern int do_readdir(int fd, struct vdir *dir);
extern int do_exec(const char *path);
extern int do_stat(const char *path, struct stat *statbuf);
extern int do_fstat(int fd, struct stat *statbuf);

#endif
