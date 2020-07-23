
#include <unistd.h>
#include <kernel/syscall.h>


int pipe(int pipefd[2])
{
	return SYSCALL1(SYS_PIPE, (int) pipefd);
}

