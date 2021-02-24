
#ifndef _SRC_KERNEL_NET_SOCKET_H
#define _SRC_KERNEL_NET_SOCKET_H

#include <kernel/vfs.h>
#include <sys/socket.h>

#define SOCKET(vnode)		(&((struct sock_vnode *) (vnode))->sock)

struct socket {
	uint8_t domain;
	uint8_t type;
	uint8_t syscall;

	struct protocol *proto;
	struct endpoint *ep;
};


int net_socket_create(int domain, int type, int protocol, uid_t uid, struct vfile **file);
int net_socket_release_vnode(struct vnode *vnode);
int net_socket_close(struct vfile *file);
int net_socket_read(struct vfile *file, char *buf, size_t nbytes);
int net_socket_write(struct vfile *file, const char *buf, size_t nbytes);
int net_socket_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid);
int net_socket_poll(struct vfile *file, int events);
int net_socket_bind(struct vfile *file, const struct sockaddr *addr, socklen_t len);
int net_socket_connect(struct vfile *file, const struct sockaddr *addr, socklen_t len);
int net_socket_listen(struct vfile *file, int n);
int net_socket_accept(struct vfile *file, struct sockaddr *addr, socklen_t *addr_len, uid_t uid, struct vfile **result);
int net_socket_shutdown(struct vfile *file, int how);
ssize_t net_socket_send(struct vfile *file, const void *buf, size_t n, int flags);
ssize_t net_socket_sendto(struct vfile *file, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len);
ssize_t net_socket_recv(struct vfile *file, void *buf, size_t n, int flags);
ssize_t net_socket_recvfrom(struct vfile *file, void *buf, size_t n, int flags, struct sockaddr *addr, socklen_t *addr_len);
int net_socket_wakeup(struct socket *sock);
int net_socket_get_options(struct vfile *file, int level, int optname, void *optval, socklen_t *optlen);
int net_socket_set_options(struct vfile *file, int level, int optname, const void *optval, socklen_t optlen);

#endif

