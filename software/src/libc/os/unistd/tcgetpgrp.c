
#include <unistd.h>
#include <sys/ioctl.h>

pid_t tcgetpgrp(int fd)
{
	return ioctl(fd, TIOCGPGRP, NULL);
}
