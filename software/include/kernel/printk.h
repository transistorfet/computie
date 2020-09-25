
#ifndef _SRC_KERNEL_PRINTK_H
#define _SRC_KERNEL_PRINTK_H

#include <stdarg.h>

int vprintk(int direct, const char *fmt, va_list args);
int printk(const char *fmt, ...);
int printk_safe(const char *fmt, ...);

#endif

