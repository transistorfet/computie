
#include <unistd.h>

int _start(int argc, char **argv, char **env)
{
	environ = env;
	return main(argc, argv, env);
}

