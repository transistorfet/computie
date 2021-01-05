
#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/types.h>

struct dirent {
	inode_t d_ino;
	char d_name[14];
};

#endif
