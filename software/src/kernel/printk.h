
#ifndef _SRC_KERNEL_PRINTK_H
#define _SRC_KERNEL_PRINTK_H

#include <stdarg.h>

int vprintk(const char *fmt, va_list args);
int printk(const char *fmt, ...);

#endif

