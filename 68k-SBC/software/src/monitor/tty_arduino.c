
#include "tty.h"


static char *tty_out = (char *) 0x2007;
static char *tty_in = (char *) 0x2007;
static char *tty_status = (char *) 0x2003;

int init_tty()
{
	return 0;
}

int getchar(void)
{
	while (1) {
		if (*tty_status & 0x01)
			return *tty_in;
	}
}

int putchar(int ch)
{
	while (!(*tty_status & 0x04)) { }
	*tty_out = ch;
	return ch;
}

