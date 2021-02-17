
#include <stdio.h>

struct _FILE {
	int fd;
};

FILE __stdin = { 0 };
FILE __stdout = { 1 };
FILE __stderr = { 2 };

