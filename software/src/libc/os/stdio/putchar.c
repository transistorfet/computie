 
#include <unistd.h>

int putchar(int ch)
{
	unsigned char c = ch;
	write(STDOUT_FILENO, &c, 1);
	return ch;
}

