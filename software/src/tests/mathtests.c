
// TODO need negative number examples

int div_pairs[][4] = {
	{ 0, 1, 0, 0 },
	{ 1, 1, 1, 0 },
	{ 98, 71, 1, 27 },
	{ 13875, 75, 185, 0 },
	{ 1029358, 12390, 83, 988 },
	{ 1029358, 98235, 10, 47008 },
	{ 0, 0, 0, 0 }
};

int mul_pairs[][3] = {
	{ 0, 1, 0 },
	{ 1, 1, 1 },
	{ 35, 456, 15960 },
	{ 124885, 235, 29347975 },
	{ 65535, 65537, 4294967295 },
	{ -436, 3, -1308 },
	{ -436, -3, 1308 },
	{ 0, 0, 0 }
};


int test_math()
{
	char ch;
	int mul, div, rem;

	puts("Div/Mod Tests");
	for (int i = 0; div_pairs[i][0] || div_pairs[i][1]; i++) {
		div = __divsi3(div_pairs[i][0], div_pairs[i][1]);
		rem = __modsi3(div_pairs[i][0], div_pairs[i][1]);

		ch = ' ';
		if (div != div_pairs[i][2] || rem != div_pairs[i][3])
			ch = 'F';
		printf("%d / %d = (%d %d) expected (%d %d)  %c\n", div_pairs[i][0], div_pairs[i][1], div, rem, div_pairs[i][2], div_pairs[i][3], ch);
	}

	puts("Mul Tests");
	for (int i = 0; mul_pairs[i][0] || mul_pairs[i][1]; i++) {
		mul = __mulsi3(mul_pairs[i][0], mul_pairs[i][1]);

		ch = ' ';
		if (mul != mul_pairs[i][2])
			ch = 'F';
		printf("%d * %d = %d expected %d  %c\n", mul_pairs[i][0], mul_pairs[i][1], mul, mul_pairs[i][2], ch);
	}

	return 0;
}

