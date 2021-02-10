
#include <stddef.h>

char **environ;

const char *getenv(const char *name)
{
	register const char *ech, *nch;
	register char **envp = environ;

	if (!envp || !name)
		return NULL;
	for (; *envp; envp++) {
		for (ech = *envp, nch = name; *nch && *ech == *nch; ech++, nch++) { }
		if (!*nch && *ech == '=')
			return ++ech;
	}
	return NULL;
}
