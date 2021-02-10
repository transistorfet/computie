
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

int tcsetattr(int fd, int optional_actions, const struct termios *tio)
{
	return ioctl(fd, TCSETS, (void *) tio);
}
