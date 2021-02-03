
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "inet_af.h"


int inet_load_sockaddr(struct sockaddr *sa, socklen_t *sa_len, struct ipv4_address *ipv4)
{
	struct sockaddr_in *sockaddr = (struct sockaddr_in *) sa;

	memset(sockaddr, '\0', *sa_len);
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = ipv4->port;
	sockaddr->sin_addr.s_addr = ipv4->addr;
	*sa_len = sizeof(struct sockaddr_in);
	return 0;
}

int inet_resolve_address(const struct sockaddr *requested, socklen_t len, const struct sockaddr *actual, struct ipv4_address *ipv4)
{
	struct sockaddr_in *requested_sin = (struct sockaddr_in *) requested;
	struct sockaddr_in *actual_sin = (struct sockaddr_in *) actual;

	// Validate the sockaddr and initialize the src address
	if (requested_sin) {
		if (len != sizeof(struct sockaddr_in))
			return EINVAL;
		if (requested_sin->sin_addr.s_addr != INADDR_ANY && requested_sin->sin_addr.s_addr != actual_sin->sin_addr.s_addr)
			return EINVAL;
		ipv4->addr = requested_sin->sin_addr.s_addr;
		ipv4->port = requested_sin->sin_port;
	}
	else {
		ipv4->addr = INADDR_ANY;
		ipv4->port = 0;
	}

	// Get the real IP and port to send from
	if (ipv4->addr == INADDR_ANY)
		ipv4->addr = actual_sin->sin_addr.s_addr;
	// TODO should this be in the udp code to ensure the ephemeral port isn't already in use?
	if (ipv4->port == 0)
		// TODO this should be an ephemeral port.  This is a temporary hack
		ipv4->port = 1284;

	return 0;
}

