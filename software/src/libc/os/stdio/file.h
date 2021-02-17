
#ifndef _OS_STDIO_FILE_H
#define _OS_STDIO_FILE_H

#define _STDIO_FF_EOF		0x01

#define _STDIO_BASE_FD		3
#define _STDIO_MAX_OPEN_FILES	20

struct _FILE {
	int fd;
	int flags;
};

extern FILE _file_table[];

#endif

