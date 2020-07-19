
#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>
#include <sys/types.h>

// WHENCE argument to lseek
#ifndef	STDIO_H			// These same definitions are in stdio.h for fseek
#define SEEK_SET	0	// Seek relative to the beginning of file
#define SEEK_CUR	1	// Seek relative to the current position
#define SEEK_END	2	// Seek relative to the end of file
#endif

#define PIPE_READ_FD	0
#define PIPE_WRITE_FD	1

struct vdir;
struct stat;

pid_t fork();
void exit(int status);
int exec(const char *path, char *const argv[], char *const envp[]);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

int open(const char *path, int flags);
int close(int fd);
size_t read(int fd, char *buf, size_t nbytes);
size_t write(int fd, const char *buf, size_t nbytes);
int readdir(int fd, struct vdir *dir);
int stat(const char *path, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);
int unlink(const char *path);
offset_t lseek(int fd, offset_t offset, int whence);
int pipe(int pipefd[2]);

#endif
