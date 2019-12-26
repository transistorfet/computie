
#include "tty.h"
//#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include <string.h>


char *led = (char *) 0x201c;


void delay(int count) {
	while (--count > 0) { }
}

int readline(char *buffer, int max)
{
	for (int i = 0; i < max; i++) {
		buffer[i] = getchar();
		if (buffer[i] == '\n') {
			buffer[++i] = '\0';
			return i;
		}
	}
	return 0;
}

int parseline(char *input, char **vargs)
{
	int j = 0;

	while (*input == ' ')
		input++;

	vargs[j++] = input;
	for (; *input != '\0' && *input != '\n'; input++) {
		if (*input == ' ') {
			*input = '\0';
			input++;
			while (*input == ' ')
				input++;
			vargs[j++] = input;
		}
	}
	*input = '\0';

	return j;
}

char hexchar(uint8_t byte)
{
	if (byte < 10)
		return byte + 0x30;
	else
		return byte + 0x37;
}

void dump(const uint8_t *addr, int len)
{
	char buffer[4];

	buffer[2] = ' ';
	buffer[3] = '\0';
	while (len > 0) {
		printf("%x: ", addr);
		for (int8_t i = 0; i < 16 && len > 0; i++, len--) {
			buffer[0] = hexchar(addr[i] >> 4);
			buffer[1] = hexchar(addr[i] & 0x0F);
			puts(buffer);
		}
		putchar('\n');
		addr -= 16;
	}
	putchar('\n');
}

void info(void)
{
	int sp;

	asm(
	"move.w	%%a7, %0\n"
	: "=r" (sp)
	);

	printf("SP: %x\n", sp);
	return;
}

#define BUF_SIZE	100
#define ARG_SIZE	10

void serial_read_loop()
{
	int argc;
	char buffer[BUF_SIZE];
	char *args[ARG_SIZE];

	while (1) {
		readline(buffer, BUF_SIZE);
		puts(buffer);
		argc = parseline(buffer, args);

		if (!strcmp(args[0], "test")) {
			puts("this is only a test\n");
		}
		else if (!strcmp(args[0], "info")) {
			info();
		}
		else if (!strcmp(args[0], "dump")) {
			if (argc <= 1)
				puts("You need an address\n");
			else {
				dump((const uint8_t *) atoi(args[1]), 0x10);
			}
		}
	}
}

int main()
{
	*led = 0x01;

	//delay(10000);

	puts("Welcome to the thing!\n");

	//snprintf(buffer, 100, "%s, or will it?\n", "this might work");
	printf("%d, or %x, or will it?\n", 1235, 0xAF5);

	serial_read_loop();

	return 0;
}

