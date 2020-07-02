
#include <stdio.h>
#include <string.h>

#define BUF_SIZE	100

int readline(char *buffer, short max)
{
	for (short i = 0; i < max; i++) {
		buffer[i] = getchar();
		putchar(buffer[i]);
		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			if (i > 0 && buffer[i - 1] == '\r')
				buffer[i - 1] = '\0';
			return i;
		}
	}
	return 0;
}

int main()
{
	char buffer[BUF_SIZE];

	putchar('\n');

	while (1) {
		putsn("% ");

		readline(buffer, BUF_SIZE);
		puts(buffer);

		if (!strcmp(buffer, "exit")) {
			break;
		}
		else if (!strcmp(buffer, "test")) {
			puts("A User Program Test");
		}
	}

	return 0;
}

