
#ifndef _TYPES_H
#define _TYPES_H

typedef long time_t;

typedef int pid_t;
typedef short device_t;

typedef unsigned short mode_t;
typedef unsigned long offset_t;
typedef unsigned int block_t;

typedef unsigned short uid_t;
typedef unsigned short gid_t;


#define O_ACCMODE	00003
#define O_RDONLY	00000
#define O_WRONLY	00001
#define O_RDWR		00002
#define O_CREAT		00100
#define O_TRUNC		01000
#define O_APPEND	02000
#define O_NONBLOCK	04000


#endif
