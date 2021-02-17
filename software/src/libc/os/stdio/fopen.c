
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "file.h"

FILE *fopen(const char *filename, const char *mode)
{
	int fd;
	FILE *stream;
	int modebits = 0;

	// Parse the first mode character or exit if an invalid mode string is found
	if (!*mode)
		return NULL;
	else if (*mode == 'w')
		modebits |= O_WRONLY | O_CREAT;
	else if (*mode == 'a')
		modebits |= O_WRONLY | O_CREAT | O_APPEND;
	else if (*mode != 'r')
		return NULL;
	mode++;

	// Parse out the optional '+' character
	if (*mode == '+') {
		modebits |= O_RDWR;
		mode++;
	}

	// Parse out the optional 'b' character
	if (*mode == 'b') {
		// TODO record open for binary (wont cause a difference atm)
		mode++;
	}

	// If the string isn't terminated here, then it must be an invalid mode string
	if (*mode)
		return NULL;

	fd = open(filename, modebits, 0644);
	if (fd < 0)
		return NULL;
	else if (fd >= _STDIO_BASE_FD + _STDIO_MAX_OPEN_FILES) {
		close(fd);
		return NULL;
	}

	stream = &_file_table[_STDIO_BASE_FD + fd];
	stream->fd = fd;

	return stream;
}

