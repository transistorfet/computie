
#include <sys/types.h>


static int system_seconds;
static int system_useconds;

void init_time()
{
	// TODO no real time clock atm
	system_seconds = 0;
	system_useconds = 0;
}

time_t get_system_time()
{
	return system_seconds;
}

void adjust_system_time(time_t usec)
{
	system_useconds += usec;
	while (system_useconds >= 1000000) {
		system_seconds += 1;
		system_useconds -= 1000000;
	}
}

