
#include <unistd.h>

#include "prototype.h"

int MAIN(command_sync)(int argc, char **argv, char **envp)
{
	sync();
}
