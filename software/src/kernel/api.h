
#ifndef _SRC_KERNEL_API_H
#define _SRC_KERNEL_API_H

#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/socket.h>

void init_syscall();
void do_syscall();

// Processes
extern void do_exit(int exitcode);
extern pid_t do_fork();
extern int do_exec(const char *path, char *const argv[], char *const envp[]);
extern pid_t do_waitpid(pid_t pid, int *status, int options);
extern int do_kill(pid_t pid, int sig);
extern int do_sigreturn();
extern int do_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
extern unsigned int do_alarm(unsigned int seconds);
extern int do_pause();
extern int do_brk(void *addr);
extern void *do_sbrk(intptr_t increment);
extern pid_t do_getpid();
extern pid_t do_getppid();
extern pid_t do_getpgid(pid_t pid);
extern int do_setpgid(pid_t pid, pid_t pgid);
extern pid_t do_getsid(pid_t pid);
extern pid_t do_setsid(void);
extern uid_t do_getuid();
extern int do_setuid(uid_t uid);

// Files & Directories
extern size_t do_read(int fd, char *buf, size_t nbytes);
extern size_t do_write(int fd, const char *buf, size_t nbytes);
extern int do_creat(const char *path, mode_t mode);
extern int do_open(const char *path, int oflags, mode_t mode);
extern int do_close(int fd);
extern int do_readdir(int fd, struct dirent *dir);
extern int do_ioctl(int fd, unsigned int request, void *argp);
extern int do_fcntl(int fd, int cmd, void *argp);
extern int do_mknod(const char *path, mode_t mode, device_t dev);
extern int do_link(const char *oldpath, const char *newpath);
extern int do_unlink(const char *path);
extern int do_mkdir(const char *path, mode_t mode);
extern char *do_getcwd(char *buf, size_t size);
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
extern mode_t do_umask(mode_t mask);
extern int do_mount(const char *source, const char *target, struct mount_opts *opts);
extern int do_umount(const char *source);
extern int do_sync();
//extern int do_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

// Time
extern time_t do_time(time_t *t);
extern int do_stime(const time_t *t);

// Sockets
extern int do_socket(int domain, int type, int protocol);
extern int do_socketpair(int domain, int type, int protocol, int fds[2]);
extern int do_connect(int fd, const struct sockaddr *addr, socklen_t len);
extern int do_bind(int fd, const struct sockaddr *addr, socklen_t len);
extern int do_listen(int fd, int n);
extern int do_accept(int fd, struct sockaddr *addr, socklen_t *addr_len);
extern int do_shutdown(int fd, int how);
extern ssize_t do_send(int fd, const void *buf, unsigned int opts[2]);
extern ssize_t do_sendto(int fd, const void *buf, unsigned int opts[4]);
extern ssize_t do_sendmsg(int fd, const struct msghdr *message, int flags);
extern ssize_t do_recv(int fd, void *buf, unsigned int opts[2]);
extern ssize_t do_recvfrom(int fd, void *buf, unsigned int opts[4]);
extern ssize_t do_recvmsg(int fd, struct msghdr *message, int flags);

//extern int do_getsockname(int fd, struct sockaddr *addr, socklen_t *len);
//extern int do_getpeername(int fd, struct sockaddr *addr, socklen_t *len);
extern int do_getsockopt(int fd, int level, unsigned int opts[3]);
extern int do_setsockopt(int fd, int level, unsigned int opts[3]);

#endif
