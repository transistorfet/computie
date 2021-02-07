
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

#define PWD_BUF_SIZE		256

int MAIN(command_pwd)(int argc, char **argv, char **envp)
{
	char buffer[PWD_BUF_SIZE];

	if (getcwd(buffer, PWD_BUF_SIZE))
		puts(buffer);
	else
		puts("Error getting pwd");

	return 0;
}

