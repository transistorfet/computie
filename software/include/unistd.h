
#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>
#include <dirent.h>
#include <sys/types.h>

#define	STDIN_FILENO	0	// Standard input
#define	STDOUT_FILENO	1	// Standard output
#define	STDERR_FILENO	2	// Standard error

// WHENCE argument to lseek
#ifndef	STDIO_H			// These same definitions are in stdio.h for fseek
#define SEEK_SET	0	// Seek relative to the beginning of file
#define SEEK_CUR	1	// Seek relative to the current position
#define SEEK_END	2	// Seek relative to the end of file
#endif

#define PIPE_READ_FD	0
#define PIPE_WRITE_FD	1

struct stat;

pid_t fork();
void exit(int status);
int execve(const char *path, char *const argv[], char *const envp[]);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
pid_t getpid();
pid_t getppid();
pid_t getpgid(pid_t pid);
int setpgid(pid_t pid, pid_t pgid);
uid_t getuid();
int kill(pid_t pid, int sig);
unsigned int alarm(unsigned int seconds);
int pause();

int mkdir(const char *path, mode_t mode);
int rename(const char *oldpath, const char *newpath);

int mknod(const char *path, mode_t mode, device_t dev);
int creat(const char *path, mode_t mode);
int open(const char *path, int flags, mode_t mode);
int close(int fd);
size_t read(int fd, char *buf, size_t nbytes);
size_t write(int fd, const char *buf, size_t nbytes);
int readdir(int fd, struct dirent *dir);
int ioctl(int fd, unsigned int request, void *argp);

int access(const char *path, int mode);
int chdir(const char *path);
int chown(const char *path, uid_t owner, gid_t group);
int chmod(const char *path, mode_t mode);
int stat(const char *path, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);
int link(const char *oldpath, const char *newpath);
int symlink(const char *target, const char *linkpath);
int unlink(const char *path);
offset_t lseek(int fd, offset_t offset, int whence);

int pipe(int pipefd[2]);
int dup2(int oldfd, int newfd);

//// Library Functions ////

int getopt(int argc, char * const argv[], const char *optstring);

#endif
