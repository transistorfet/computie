
#include <stdlib.h>

static unsigned int rand_next = 1;

void srand(unsigned int seed)
{
	rand_next = seed;
}

int rand(void)
{
	rand_next = rand_next * 1103515245 + 12345;
	return (rand_next >> 16) & RAND_MAX;
}

