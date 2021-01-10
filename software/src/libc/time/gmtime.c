
#include <time.h>

#define SEC_PER_HOUR		(60 * 60)
#define	LEAPYEAR(year)		(!((year) % 4) && (((year) % 100) || !((year) % 400)))

static char _monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static struct tm _time_data;

struct tm *gmtime(const time_t *t)
{
	time_t time = *t;
	register struct tm *tp = &_time_data;

	// TODO the complexity here is because my multiply and modulus functions are broken and can only work on 32bit numerators and 16bit divisors
	register int hournum = time / SEC_PER_HOUR;
	register int hoursec = time % SEC_PER_HOUR;
	register int daynum = hournum / 24;
	register int daysec = (hournum % 24) * SEC_PER_HOUR + hoursec;

	tp->tm_sec = daysec % 60;
	tp->tm_min = (daysec % 3600) / 60;
	tp->tm_hour = daysec / 3600;
	tp->tm_wday = (daynum + 4) % 7;			// day 0 was a thursday
	//tp->tm_year = 1970 + ((daynum / 1461) * 4);	// 1461 = 365.25 * 4

	tp->tm_year = 1970;
	tp->tm_yday = daynum;
	while (1) {
		register int days = LEAPYEAR(tp->tm_year) ? 366 : 365;
		if (tp->tm_yday < days)
			break;
		tp->tm_yday -= days;
		tp->tm_year++;
	}

	tp->tm_mon = 0;
	tp->tm_mday = tp->tm_yday + 1;
	while (1) {
		register int days = (tp->tm_mon == 1 && LEAPYEAR(tp->tm_year)) ? _monthdays[tp->tm_mon] + 1 : _monthdays[tp->tm_mon];
		if (tp->tm_mday < days)
			break;
		tp->tm_mday -= days;
		tp->tm_mon++;
	}
	tp->tm_isdst = 0;

	return tp;
}

