
#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <string.h>
#include <sys/types.h>

#define _FDS_MAX	128

typedef long int fd_mask_t;

#define __FDS_BITS		(8 * sizeof(fd_mask_t))
#define __FDS_LOG_BITS		__builtin_ctz(__FDS_BITS)
#define __FDS_MASK_BITS		(__FDS_BITS - 1)

typedef struct {
	fd_mask_t fds_bits[_FDS_MAX / __FDS_BITS];
} fd_set;


#define	FD_SET(fd, fdsetp)	((fdsetp)->fds_bits[(fd) >> __FDS_LOG_BITS] |=  (0x01 << ((fd) & __FDS_MASK_BITS)))
#define	FD_CLR(fd, fdsetp)	((fdsetp)->fds_bits[(fd) >> __FDS_LOG_BITS] &= ~(0x01 << ((fd) & __FDS_MASK_BITS)))
#define	FD_ISSET(fd, fdsetp)	((fdsetp)->fds_bits[(fd) >> __FDS_LOG_BITS] &   (0x01 << ((fd) & __FDS_MASK_BITS)))
#define	FD_ZERO(fdsetp)		(memset((fdsetp), '\0', sizeof(fd_set)))


struct timeval {
	time_t tv_sec;
	long tv_usec;
};


int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#endif

