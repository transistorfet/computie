
#ifndef _SYS_IOC_TTY_H
#define _SYS_IOC_TTY_H

#include <termios.h>
#include <sys/ioc_common.h>

#define TCGETS		_IOR('T',  8, struct termios)	// tcgetattr
#define TCSETS		_IOW('T',  9, struct termios)	// tcsetattr, TCSANOW
#define TCSETSW		_IOW('T', 10, struct termios)	// tcsetattr, TCSADRAIN
#define TCSETSF		_IOW('T', 11, struct termios)	// tcsetattr, TCSAFLUSH
#define	TIOCGPGRP	_IOW('T', 18, int)
#define	TIOCSPGRP	_IOW('T', 19, int)

#endif
