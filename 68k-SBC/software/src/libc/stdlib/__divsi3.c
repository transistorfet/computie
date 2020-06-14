

#ifdef __m68k__
// TODO this only supports a positive 32bit numerator and a positive 16bit denominator
int __divsi3(int a, int b)
{
	int c;

	asm(
	"clr.l	%%d0\n"
	"clr.l	%%d1\n"
	"move.w	%1, %%d0\n"	// d0 <- low a
	"move.w	%2, %%d1\n"	// d1 <- low b
	"move.l %1, %%d2\n"
	"clr.w	%%d2\n"
	"swap	%%d2\n"		// d2 <- high a

	"divu	%%d1, %%d2\n"
	"move.w %%d2, %%d3\n"
	"swap	%%d3\n"
	"clr.w	%%d3\n"		// d3 <- high result
	"clr.w	%%d2\n"		// d2 <- remainder << 16
	"add.l	%%d2, %%d0\n"	// d0 <- (remainder << 16) + low a

	"divu	%%d1, %%d0\n"

	"swap	%%d0\n"
	"clr.w	%%d0\n"
	"swap	%%d0\n"		// clear upper d0 (throw away remainder)

	"add.l	%%d3, %%d0\n"
	"move.l	%%d0, %0\n"
	: "=g" (c)
	: "g" (a), "g" (b)
	: "%d0", "%d1", "%d2", "%d3"
	);
	return c;
}

/*
int __divsi3(int a, int b)
{
	int result = 0;
	while (1) {
		a -= b;
		if (a < 0)
			break;
		result++;
	}
	return result;
}
*/
#endif


