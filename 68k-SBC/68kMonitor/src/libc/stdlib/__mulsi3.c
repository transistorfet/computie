

#ifdef __m68k__
/*
int __mulsi3(int a, int b)
{
	int c;

	asm(
	"move.w	%1, %%d0\n"
	"move.w	%2, %%d1\n"
	"muls	%%d1, %%d0\n"
	"move.w	%%d0, %0\n"
	: "=r" (c)
	: "r" (a), "r" (b)
	: "%d0", "%d1"
	);
	return c;
}
*/

#include <stdint.h>

int __mulsi3(int a, int b)
{
	/*
	uint16_t ah = (uint16_t) (a >> 16);
	uint16_t al = (uint16_t) a;
	uint16_t bh = (uint16_t) (b >> 16);
	uint16_t bl = (uint16_t) b;

	return ((ah * bh) << 32) + (((ah * bl) + (bh * al)) << 16) + (al * bl);
	*/

	int result = 0;
	for (; b > 0; b--) {
		result += a;
	}
	return result;
}
#endif


