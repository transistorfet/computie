 
#ifndef _ASM_M68K_MACROS_H
#define _ASM_M68K_MACROS_H

#define NOP()			asm volatile("nop\n");

#define ENDIAN_CONVERT_WORD(x)	asm volatile("rol.w	#8, %0\n" : "+g" ((x)));
#define ENDIAN_CONVERT_LONG(x) { 	\
	asm volatile(			\
	"rol.w	#8, %0\n"		\
	"swap	%0\n"			\
	"rol.w	#8, %0\n"		\
	: "+r" ((x))			\
	);				\
}

static inline short endian_convert_word(short x)
{
	asm volatile("rol.w	#8, %0\n" : "+md" (x));
	return x;
}

static inline int endian_convert_long(int x)
{
	asm volatile(
	"rol.w	#8, %0\n"
	"swap	%0\n"
	"rol.w	#8, %0\n"
	: "+md" (x)
	);
	return x;
}

#define to_le16(x)	endian_convert_word((x))
#define to_le32(x)	endian_convert_long((x))
#define to_be16(x)	((x))
#define to_be32(x)	((x))
#define from_le16(x)	endian_convert_word((x))
#define from_le32(x)	endian_convert_long((x))
#define from_be16(x)	((x))
#define from_be32(x)	((x))

#endif
