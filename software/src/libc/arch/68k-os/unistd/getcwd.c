
#include <unistd.h>
#include <kernel/syscall.h>

char *getcwd(char *buf, size_t size)
{
	return (char *) SYSCALL2(SYS_GETCWD, (int) buf, (int) size);
}

