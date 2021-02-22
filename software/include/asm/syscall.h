 
#ifndef _ASM_SYSCALL_H
#define _ASM_SYSCALL_H

#ifdef __m68k__
#include <asm/m68k/syscall.h>
#else
#error "No architecture detected\n"
#endif

#endif
