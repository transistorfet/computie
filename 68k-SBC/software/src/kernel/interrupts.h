
#ifndef _SRC_KERNEL_INTERRUPTS_H
#define _SRC_KERNEL_INTERRUPTS_H


#define IV_BUS_ERROR		2
#define IV_ADDRESS_ERROR	3
#define IV_ILLEGAL_INSTRUCTION	4
#define IV_ZERO_DIVIDE		5
#define IV_CHK_INSTRUCTION	6
#define IV_TRAPV_INSTRUCTION	7
#define IV_PRIVILEGE_VIOLATION	8
#define IV_TRACE		9
#define IV_LINE_1010_EMULATOR	10
#define IV_LINE_1111_EMULATOR	11

#define IV_FORMAT_ERROR		14
#define IV_UNINITIALIZED_VEC	15

#define IV_TRAP0		32
#define IV_TRAP1		33
#define IV_TRAP2		34
#define IV_TRAP3		35
#define IV_TRAP4		36
#define IV_TRAP5		37
#define IV_TRAP6		38
#define IV_TRAP7		39
#define IV_TRAP8		40
#define IV_TRAP9		41
#define IV_TRAP10		42
#define IV_TRAP11		43
#define IV_TRAP12		44
#define IV_TRAP13		45
#define IV_TRAP14		46
#define IV_TRAP15		47

#define IV_USER_VECTORS		64

/*** Macros ***/

#define DISABLE_INTS()		asm volatile("or.w	#0x0700, %sr");
#define ENABLE_INTS()		asm volatile("and.w	#0xF8FF, %sr");

#define TRACE_ON()		asm volatile("or.w	#0x8000, %sr");
#define TRACE_OFF()		asm volatile("and.w	#0x7FFF, %sr");

#define ARDUINO_TRACE_ON()	asm volatile("movea.l	#0x2019, %%a0\n" "move.b	#1, (%%a0)" : : : "%a0");
#define ARDUINO_TRACE_OFF()	asm volatile("movea.l	#0x2019, %%a0\n" "move.b	#0, (%%a0)" : : : "%a0");


typedef void (*interrupt_handler_t)();

void init_interrupts();
void set_interrupt(char iv_num, interrupt_handler_t handler);
void panic(const char *fmt, ...);

#endif

