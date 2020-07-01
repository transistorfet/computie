
#ifndef _SRC_KERNEL_API_H
#define _SRC_KERNEL_API_H

extern void init_syscall();

extern int do_exit();
extern int do_fork();
extern size_t do_read(int fd, char *buf, size_t nbytes);
extern size_t do_write(int fd, const char *buf, size_t nbytes);
extern int do_open(const char *path, int oflags);
extern int do_close(int fd);

#endif
