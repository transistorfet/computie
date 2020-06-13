
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

typedef void (*interrupt_handler_t)();


#endif

