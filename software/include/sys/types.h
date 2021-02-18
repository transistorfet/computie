
#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

typedef long time_t;
typedef long clock_t;

typedef int pid_t;
typedef short device_t;

typedef unsigned short mode_t;
typedef unsigned long offset_t;
typedef unsigned int block_t;
typedef unsigned int zone_t;
typedef unsigned int inode_t;
typedef unsigned int nlink_t;

typedef unsigned short uid_t;
typedef unsigned short gid_t;

#define SU_UID		(uid_t) 0


// Standard type names
typedef device_t dev_t;
typedef inode_t ino_t;
typedef offset_t off_t;

#endif
