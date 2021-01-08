
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

int tcgetattr(int fd, struct termios *tio)
{
	return ioctl(fd, TCGETS, tio);
}
