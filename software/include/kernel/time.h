
#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <sys/types.h>

void init_time();
void set_system_time(time_t t);
time_t get_system_time();
void adjust_system_time(time_t usec);

#endif
