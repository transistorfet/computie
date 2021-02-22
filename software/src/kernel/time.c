
#include <sys/types.h>


static time_t system_seconds;
static time_t system_uptime;
static time_t system_useconds;

void init_time()
{
	// TODO no real time clock atm
	system_seconds = 0;
	system_uptime = 0;
	system_useconds = 0;
}

void set_system_time(time_t t)
{
	system_seconds = t;
}

time_t get_system_time()
{
	return system_seconds;
}

void get_system_uptime(time_t uptime[2])
{
	uptime[0] = system_uptime;
	uptime[1] = system_useconds;
}

void adjust_system_time(time_t usec)
{
	system_useconds += usec;
	while (system_useconds >= 1000000) {
		system_useconds -= 1000000;
		system_seconds += 1;
		system_uptime += 1;
	}
}

