
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <kernel/vfs.h>
#include <kernel/kmalloc.h>
#include <kernel/syscall.h>
#include <kernel/scheduler.h>

// TODO this is IP-specific and probably shouldn't be here
#include <netinet/in.h>

#include "../fs/nop.h"
#include "../fs/fileptr.h"

#include "if.h"
#include "packet.h"
#include "socket.h"
#include "protocol.h"


struct sock_vnode {
	struct vnode vn;
	struct socket sock;
};


struct vfile_ops sock_vfile_ops = {
	nop_open,
	net_socket_close,
	net_socket_read,
	net_socket_write,
	net_socket_ioctl,
	net_socket_poll,
	nop_seek,
	nop_readdir,
};

struct vnode_ops sock_vnode_ops = {
	&sock_vfile_ops,
	nop_create,
	nop_mknod,
	nop_lookup,
	nop_link,
	nop_unlink,
	nop_rename,
	nop_truncate,
	nop_update,
	net_socket_release_vnode,
};


int net_socket_create(int domain, int type, int protocol, uid_t uid, struct vfile **file)
{
	struct protocol *proto;
	struct sock_vnode *vnode;

	proto = net_get_protocol(domain, type, protocol);
	if (!proto)
		return EAFNOSUPPORT;

	vnode = kmalloc(sizeof(struct sock_vnode));
	if (!vnode)
		return NULL;
	vfs_init_vnode(&vnode->vn, &sock_vnode_ops, NULL, S_IFSOCK | 0600, 1, uid, 0, 0, 0, 0, 0, 0, 0);

	*file = new_fileptr(&vnode->vn, 0);
	if (!*file) {
		kmfree(vnode);
		return EMFILE;
	}

	vnode->sock.domain = domain;
	vnode->sock.type = type;
	vnode->sock.syscall = 0;

	vnode->sock.proto = proto;
	vnode->sock.ep = NULL;

	return 0;
}

int net_socket_release_vnode(struct vnode *vnode)
{
	struct socket *sock = SOCKET(vnode);

	if (sock->ep) {
		net_destroy_endpoint(sock->ep);
		sock->ep = NULL;
	}

	kmfree(vnode);
	return 0;
}

int net_socket_close(struct vfile *file)
{
	struct socket *sock = SOCKET(file->vnode);

	if (sock->ep) {
		net_destroy_endpoint(sock->ep);
		sock->ep = NULL;
	}

	return 0;
}

int net_socket_bind(struct vfile *file, const struct sockaddr *addr, socklen_t len)
{
	struct socket *sock = SOCKET(file->vnode);

	if (!addr || addr->sa_family != sock->domain)
		return EAFNOSUPPORT;
	if (sock->ep)
		return EISCONN;

	return net_create_endpoint(sock->proto, sock, addr, len, &sock->ep);
}

int net_socket_connect(struct vfile *file, const struct sockaddr *addr, socklen_t len)
{
	int result;
	struct socket *sock = SOCKET(file->vnode);

	if (sock->ep)
		return EISCONN;

	int error = net_create_endpoint(sock->proto, sock, NULL, 0, &sock->ep);
	if (error)
		return error;

	if (!sock->ep->ops->connect)
		// TODO should you free the endpoint here?  It will still be freed when the socket is closed
		return EAFNOSUPPORT;

	result = sock->ep->ops->connect(sock->ep, addr, len);
	// TODO restarting the connect syscall would fail because we are connected, so this wont work
	//if (result == EWOULDBLOCK) {
	//	sock->syscall = SYS_CONNECT;
	//	suspend_current_syscall();
	//	return 0;
	//}
	return result;
}

int net_socket_listen(struct vfile *file, int n)
{
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->listen)
		return EAFNOSUPPORT;
	return sock->ep->ops->listen(sock->ep, n);
}

int net_socket_accept(struct vfile *file, struct sockaddr *addr, socklen_t *addr_len, uid_t uid, struct vfile **result)
{
	int error;
	struct endpoint *ep;
	struct vfile *newfile;
	struct socket *client;
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->accept)
		return EAFNOSUPPORT;

	error = sock->ep->ops->accept(sock->ep, addr, addr_len, &ep);
	if (error == EWOULDBLOCK) {
		sock->syscall = SYS_ACCEPT;
		suspend_current_syscall();
		return 0;
	}
	else if (error)
		return error;

	error = net_socket_create(sock->domain, sock->type, sock->proto->protocol, uid, &newfile);
	if (error)
		return error;

	client = SOCKET(newfile->vnode);
	client->ep = ep;
	ep->sock = client;

	*result = newfile;
	return 0;
}

int net_socket_shutdown(struct vfile *file, int how)
{
	// TODO implement
	return -1;
}

int net_socket_read(struct vfile *file, char *buf, size_t nbytes)
{
	if (!S_ISSOCK(file->vnode->mode))
		return EBADF;
	return net_socket_recv(file, buf, nbytes, 0);
}

int net_socket_write(struct vfile *file, const char *buf, size_t nbytes)
{
	if (!S_ISSOCK(file->vnode->mode))
		return EBADF;
	return net_socket_send(file, buf, nbytes, 0);
}

ssize_t net_socket_send(struct vfile *file, const void *buf, size_t n, int flags)
{
	int result;
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->send)
		return EAFNOSUPPORT;

	result = sock->ep->ops->send(sock->ep, buf, n);
	if (result == EWOULDBLOCK) {
		sock->syscall = SYS_SEND;
		suspend_current_syscall();
		return 0;
	}
	return result;
}

ssize_t net_socket_recv(struct vfile *file, void *buf, size_t n, int flags)
{
	int result;
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->recv)
		return EAFNOSUPPORT;

	result = sock->ep->ops->recv(sock->ep, buf, n);
	if (result == EWOULDBLOCK) {
		sock->syscall = SYS_RECV;
		suspend_current_syscall();
		return 0;
	}

	return result;
}

ssize_t net_socket_sendto(struct vfile *file, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len)
{
	struct socket *sock = SOCKET(file->vnode);

	if (addr->sa_family != sock->domain)
		return EINVAL;

	// If no endpoint is open, then create an ephemeral endpoint
	if (!sock->ep) {
		int error = net_create_endpoint(sock->proto, sock, NULL, 0, &sock->ep);
		if (error)
			return error;
	}

	if (!sock->ep->ops->send_to)
		return EAFNOSUPPORT;

	return sock->ep->ops->send_to(sock->ep, buf, n, addr, addr_len);
}

ssize_t net_socket_recvfrom(struct vfile *file, void *buf, size_t n, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
	int result;
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->recv_from)
		return EAFNOSUPPORT;

	result = sock->ep->ops->recv_from(sock->ep, buf, n, addr, addr_len);
	if (result == EWOULDBLOCK) {
		sock->syscall = SYS_RECVFROM;
		suspend_current_syscall();
		return 0;
	}
	return result;
}

int net_socket_wakeup(struct socket *sock)
{
	struct vnode *vnode = (struct vnode *) (((char *) sock) - sizeof(struct vnode));
	resume_blocked_procs(sock->syscall, vnode, 0);
	sock->syscall = 0;
	return 0;
}

int net_socket_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid)
{
	struct socket *sock = SOCKET(file->vnode);

	if (!S_ISSOCK(file->vnode->mode))
		return EBADF;

	switch (request) {

		default:
			return net_if_ioctl(request, (struct ifreq *) argp, uid);
	}
	return 0;
}

int net_socket_poll(struct vfile *file, int events)
{
	struct socket *sock = SOCKET(file->vnode);

	if (!S_ISSOCK(file->vnode->mode))
		return EBADF;
	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->poll)
		return EAFNOSUPPORT;
	return sock->ep->ops->poll(sock->ep, events);
}

int net_socket_get_options(struct vfile *file, int level, int optname, void *optval, socklen_t *optlen)
{
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->get_options)
		return EAFNOSUPPORT;
	return sock->ep->ops->get_options(sock->ep, level, optname, optval, optlen);
}

int net_socket_set_options(struct vfile *file, int level, int optname, const void *optval, socklen_t optlen)
{
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->ep)
		return ENOTCONN;
	if (!sock->ep->ops->set_options)
		return EAFNOSUPPORT;
	return sock->ep->ops->set_options(sock->ep, level, optname, optval, optlen);
}

