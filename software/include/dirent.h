
#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/types.h>

struct dirent {
	inode_t ino;
	char name[14];
};

#endif
