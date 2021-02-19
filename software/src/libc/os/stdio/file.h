
#ifndef _OS_STDIO_FILE_H
#define _OS_STDIO_FILE_H

#define _STDIO_BASE_FD		3
#define _STDIO_MAX_OPEN_FILES	FOPEN_MAX

struct _FILE {
	int fd;
	short flags;
	short bufsize;
	short count;
	short pos;
	char *buf;
};

extern FILE _file_table[];

#endif

