
#ifndef _SYS_IOC_IF_H
#define _SYS_IOC_IF_H

#include <sys/socket.h>
#include <sys/ioc_common.h>

#define IFF_UP			0x01	// Interface is running
#define IFF_BROADCAST		0x02	// Valid broadcast address set
#define IFF_DEBUG		0x04	// Internal debugging flag

#define IFNAMESIZE		16

struct ifreq {
	char ifr_name[IFNAMESIZE];
	union {
		struct sockaddr_storage ifr_addr;
		struct sockaddr_storage ifr_dstaddr;
		struct sockaddr_storage ifr_broadaddr;
		struct sockaddr_storage ifr_netmask;
		struct sockaddr_storage ifr_hwaddr;

		short ifr_flags;
		int ifr_metric;
		int ifr_mtu;
	};
};

#define SIOCGIFFLAGS	_IOR('I',  10, struct ifreq)
#define SIOCSIFFLAGS	_IOW('I',  11, struct ifreq)
#define SIOCGIFADDR	_IOR('I',  12, struct ifreq)
#define SIOCSIFADDR	_IOW('I',  13, struct ifreq)

#endif

