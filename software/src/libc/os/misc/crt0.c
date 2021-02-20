
#include <unistd.h>

extern int main(int argc, char **argv, char **env);

int _start(int argc, char **argv, char **env)
{
	environ = env;
	return main(argc, argv, env);
}

