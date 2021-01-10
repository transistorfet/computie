
#ifndef _TIME_H
#define _TIME_H

#include <stddef.h>
#include <sys/types.h>

struct tm {
	int tm_sec;			// seconds after the minute [0, 59]
	int tm_min;			// minutes after the hour [0, 59]
	int tm_hour;			// hours since midnight [0, 23]
	int tm_mday;			// day of the month [1, 31]
	int tm_mon;			// months since January [0, 11]
	int tm_year;			// years since 1900
	int tm_wday;			// days since Sunday [0, 6]
	int tm_yday;			// days since January 1 [0, 365]
	int tm_isdst;			// Daylight Saving Time flag
};

//extern char *tzname[];

time_t time(time_t *t);
int stime(const time_t *t);
struct tm *gmtime(const time_t *t);
size_t strftime (char* buffer, size_t max, const char *fmt, const struct tm *tp);

#endif

