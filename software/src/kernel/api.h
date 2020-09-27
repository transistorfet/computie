
#ifndef _SRC_KERNEL_API_H
#define _SRC_KERNEL_API_H

#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void init_syscall();
void do_syscall();

// Processes
extern void do_exit(int exitcode);
extern pid_t do_fork();
extern int do_exec(const char *path, char *const argv[], char *const envp[]);
extern pid_t do_wait(int *status);
extern pid_t do_waitpid(pid_t pid, int *status, int options);
extern pid_t do_getpid();
extern pid_t do_getppid();
extern uid_t do_getuid();

// Files & Directories
extern size_t do_read(int fd, char *buf, size_t nbytes);
extern size_t do_write(int fd, const char *buf, size_t nbytes);
extern int do_creat(const char *path, mode_t mode);
extern int do_open(const char *path, int oflags, mode_t mode);
extern int do_close(int fd);
extern int do_readdir(int fd, struct dirent *dir);
extern int do_unlink(const char *path);
extern int do_mkdir(const char *path, mode_t mode);
extern int do_rename(const char *oldpath, const char *newpath);
extern int do_chdir(const char *path);
extern int do_access(const char *path, int mode);
extern int do_chown(const char *path, uid_t owner, gid_t group);
extern int do_chmod(const char *path, int mode);
extern int do_stat(const char *path, struct stat *statbuf);
extern int do_fstat(int fd, struct stat *statbuf);
extern int do_lseek(int fd, offset_t offset, int whence);
extern int do_pipe(int pipefd[2]);
extern int do_dup2(int oldfd, int newfd);

// Time
extern time_t do_time(struct tm *tptr);

#endif
