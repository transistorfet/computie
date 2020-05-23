

#ifdef __m68k__
/*
int __modsi3(int a, int b)
{
	int c;

	asm(
	"move.w	%1, %%d0\n"
	"move.w	%2, %%d1\n"
	"divs	%%d1, %%d0\n"
	"swap	%%d0\n"
	"move.w	%%d0, %0\n"
	: "=r" (c)
	: "r" (a), "r" (b)
	: "%d0", "%d1"
	);
	return c;
}
*/

int __modsi3(int a, int b)
{
	while (1) {
		if (a < b)
			break;
		a -= b;
	}
	return a;
}
#endif


