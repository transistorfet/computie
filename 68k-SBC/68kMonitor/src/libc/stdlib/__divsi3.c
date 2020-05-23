

#ifdef __m68k__
/*
int __divsi3(int a, int b)
{
	int c;

	asm(
	"move.w	%1, %%d0\n"
	"move.w	%2, %%d1\n"
	"divs	%%d1, %%d0\n"
	"move.w	%%d0, %0\n"
	: "=r" (c)
	: "r" (a), "r" (b)
	: "%d0", "%d1"
	);
	return c;
}
*/

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
#endif


