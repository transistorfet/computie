
#ifndef _TYPES_H
#define _TYPES_H

#define F_OK               0	// Test if file exists
#define X_OK               1	// Test if file is executable
#define W_OK               2	// Test if file is writable
#define R_OK               4	// Test if file is readable

typedef int time_t;

typedef int pid_t;
typedef short device_t;

typedef unsigned short mode_t;
typedef unsigned long offset_t;
typedef unsigned int block_t;
typedef unsigned int zone_t;
typedef unsigned int inode_t;

typedef unsigned short uid_t;
typedef unsigned short gid_t;

#define SU_UID		(uid_t) 0

#define O_ACCMODE	00003
#define O_RDONLY	00000
#define O_WRONLY	00001
#define O_RDWR		00002
#define O_CREAT		00100
#define O_TRUNC		01000
#define O_APPEND	02000
#define O_NONBLOCK	04000


#endif
