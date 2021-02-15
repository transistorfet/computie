
#ifndef _SOCKET_H
#define _SOCKET_H

#include <stddef.h>

#define SOCK_STREAM		1
#define SOCK_DGRAM		2
#define SOCK_RAW		3

// Protocol Families
#define PF_UNSPEC		0		// Unspecified
#define PF_UNIX			1		// Pipes and file-domain
#define PF_LOCAL		PF_LOCAL	// Other name for PF_UNIX
#define PF_INET			2		// IP version 4
#define PF_INET6		3		// IP version 6

// Address Families
#define AF_UNSPEC		PF_UNSPEC
#define AF_LOCAL		PF_UNIX
#define AF_UNIX			PF_UNIX
#define AF_INET			PF_INET
#define AF_INET6		PF_INET6

#define SHUT_RD			0
#define SHUT_WR			1
#define SHUT_RDWR		2

// Socket Options
#define SO_DEBUG		4
#define SO_GETSOCKNAME		128
#define SO_GETPEERNAME		129


typedef unsigned int socklen_t;

typedef unsigned short sa_family_t;

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
};

// TODO this is supposed to be the largest possible sockaddr.  Right now it's set to the size of struct sockaddr, but if IPv6 addrs were supported, it would need to be bigger
#define _SA_STORAGE_SIZE	16

struct sockaddr_storage {
	sa_family_t ss_family;
	char _padding[_SA_STORAGE_SIZE - sizeof(sa_family_t)];
};


struct iovec;

struct msghdr {
	void *msg_name;			// optional address
	socklen_t msg_namelen;		// size of address
	struct iovec *msg_iov;		// scatter/gather array
	int msg_iovlen;			// members in msg_iov
	void *msg_control;		// ancillary data, see below
	socklen_t msg_controllen;	// ancillary data buffer len
	int msg_flags;			// flags on received message
};

int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int fds[2]);

int connect(int fd, const struct sockaddr *addr, socklen_t len);
int bind(int fd, const struct sockaddr *addr, socklen_t len);
int listen(int fd, int n);
int accept(int fd, struct sockaddr *addr, socklen_t *addr_len);
int shutdown(int fd, int how);

ssize_t send(int fd, const void *buf, size_t n, int flags);
ssize_t sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len);
ssize_t sendmsg(int fd, const struct msghdr *message, int flags);

ssize_t recv(int fd, void *buf, size_t n, int flags);
ssize_t recvfrom(int fd, void *buf, size_t n, int flags, struct sockaddr *addr, socklen_t *addr_len);
ssize_t recvmsg(int fd, struct msghdr *message, int flags);

int getsockname(int fd, struct sockaddr *addr, socklen_t *len);
int getpeername(int fd, struct sockaddr *addr, socklen_t *len);
int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

#endif

