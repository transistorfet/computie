
#ifndef _SYS_IOC_TTY_H
#define _SYS_IOC_TTY_H

#include <sys/ioc_common.h>

#define	TIOCGPGRP	_IOW('T', 18, int)
#define	TIOCSPGRP	_IOW('T', 19, int)

#endif
