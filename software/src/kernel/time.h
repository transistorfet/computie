
#ifndef _SRC_KERNEL_TIME_H
#define _SRC_KERNEL_TIME_H

#include <sys/types.h>

void init_time();
time_t get_system_time();
void adjust_system_time(time_t usec);

#endif
