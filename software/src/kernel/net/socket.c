
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
	nop_ioctl,
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

	proto = get_protocol(domain, type, protocol);
	if (!proto)
		return EAFNOSUPPORT;

	vnode = kmalloc(sizeof(struct sock_vnode));
	if (!vnode)
		return NULL;
	vfs_init_vnode(&vnode->vn, &sock_vnode_ops, NULL, S_IFSOCK | 0600, 1, uid, 0, 0, 0, 0, 0, 0);

	*file = new_fileptr(&vnode->vn, 0);
	if (!*file) {
		kmfree(vnode);
		return EMFILE;
	}

	_queue_node_init(&vnode->sock.node);
	vnode->sock.domain = domain;
	vnode->sock.type = type;
	vnode->sock.state = 0;
	_queue_init(&vnode->sock.recv_queue);

	memset(&vnode->sock.local, '\0', sizeof(struct sockaddr_storage));
	memset(&vnode->sock.remote, '\0', sizeof(struct sockaddr_storage));

	vnode->sock.proto = proto;

	return 0;
}

int net_socket_release_vnode(struct vnode *vnode)
{
	remove_protocol_listener(SOCKET(vnode)->proto, SOCKET(vnode));

	// TODO should this go in close instead?
	for (struct queue_node *next, *cur = SOCKET(vnode)->recv_queue.head; cur; cur = next) {
		next = cur->next;
		packet_free((struct packet *) cur);
	}

	kmfree(vnode);
	return 0;
}

int net_socket_close(struct vfile *file)
{
	// TODO close connection
	return 0;
}

int net_socket_bind(struct vfile *file, const struct sockaddr *addr, socklen_t len)
{
	struct socket *sock = SOCKET(file->vnode);

	// TODO how do you replace the sockaddr_in sizeof ref here?
	if (!addr || addr->sa_family != sock->domain || len != sizeof(struct sockaddr_in))
		return EAFNOSUPPORT;

	memcpy(&sock->local, addr, len);

	return add_protocol_listener(sock->proto, sock);
}

int net_socket_connect(struct vfile *file, const struct sockaddr *addr, socklen_t len)
{

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

}

ssize_t net_socket_sendto(struct vfile *file, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len)
{
	struct packet *pack;
	struct if_device *ifdev;
	struct socket *sock = SOCKET(file->vnode);

	if (addr->sa_family != sock->domain)
		return EINVAL;

	// TODO this is a temporary hack
	ifdev = net_if_find("slip0");

	pack = create_protocol_packet(sock->proto, (struct sockaddr *) &sock->local, addr, buf, n);
	net_if_send_packet(ifdev, pack);
	return n;
}

ssize_t net_socket_recv(struct vfile *file, void *buf, size_t n, int flags)
{

}

ssize_t net_socket_recvfrom(struct vfile *file, void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t *addr_len)
{
	struct packet *pack;
	struct socket *sock = SOCKET(file->vnode);

	if (!sock->recv_queue.head) {
		sock->syscall = SYS_RECVFROM;
		suspend_current_proc();
		return 0;
	}

	pack = (struct packet *) sock->recv_queue.head;
	_queue_remove(&sock->recv_queue, sock->recv_queue.head);

	if (pack->length - pack->data_offset < n)
		n = pack->length - pack->data_offset;
	memcpy(buf, &pack->data[pack->data_offset], n);

	if (addr) {
		// TODO copy address
		
	}

	packet_free(pack);
	return n;
}

int net_socket_wakeup(struct socket *sock)
{
	struct vnode *vnode = (struct vnode *) (((char *) sock) - sizeof(struct vnode));
	resume_blocked_procs(sock->syscall, vnode, 0);
	return 0;
}

