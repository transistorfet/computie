

#ifdef __m68k__
int __mulsi3(int a, int b)
{
	int c;

	asm(
	"move.l	%1, %%d0\n"	// d0 <- low a
	"move.l	%%d0, %%d1\n"
	"swap	%%d1\n"		// d1 <- high a
	"move.l	%2, %%d2\n"	// d2 <- low b
	"move.l	%%d2, %%d3\n"
	"swap	%%d3\n"		// d3 <- high b

	"mulu.w	%%d2, %%d0\n"	// d0 <- low b * high b
	"mulu.w %%d1, %%d2\n"	// d2 <- high a * low b
	"move.w	%1, %%d1\n"
	"mulu.w	%%d3, %%d1\n"	// d1 <- high b * low a

	"swap	%%d1\n"		// d1 << 16
	"clr.w	%%d1\n"
	"swap	%%d2\n"		// d2 << 16
	"clr.w	%%d2\n"

	"add.l	%%d2,%%d0\n"	// add up the results
	"add.l	%%d1,%%d0\n"
	"move.l	%%d0, %0\n"
	: "=r" (c)
	: "r" (a), "r" (b)
	: "%d0", "%d1", "%d2", "%d3"
	);
	return c;
}

	/*
#include <stdint.h>

int __mulsi3(int a, int b)
{
	int16_t ah = (int16_t) (a >> 16);
	int16_t al = (int16_t) a;
	int16_t bh = (int16_t) (b >> 16);
	int16_t bl = (int16_t) b;

	return (((int16_t) (ah * bl) + (int16_t) (bh * al)) << 16) + (int16_t) (al * bl);

	int result = 0;
	for (; b > 0; b--) {
		result += a;
	}
	return result;
}
	*/
#endif


