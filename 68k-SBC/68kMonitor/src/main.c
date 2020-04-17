
#include "tty.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


char *led = (char *) 0x201c;


void delay(int count) {
	while (--count > 0) { }
}

int readline(char *buffer, int max)
{
	for (int i = 0; i < max; i++) {
		buffer[i] = getchar();
		if (buffer[i] == '\n' || buffer[i] == '\r') {
			buffer[i] = '\n';
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
	int sv1;

	asm(
	"move.w	%%a7, %0\n"
	"move.w	(%%a7), %1\n"
	: "=r" (sp), "=r" (sv1)
	);

	printf("SP: %x\n", sp);
	printf("TOP: %x\n", sv1);
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
				dump((const uint8_t *) strtol(args[1], NULL, 16), 0x10);
			}
		}
	}
}

int main()
{
	*led = 0x01;
	init_tty();

	//delay(10000);

	puts("Welcome to the thing!\n");

	//snprintf(buffer, 100, "%s, or will it?\n", "this might work");
	//printf("Test %x\n", 0xAF5);

	uint16_t *mem = (uint16_t *) 0x100000;
	for (int i = 0; i < 20; i++) {
		mem[i] = (uint16_t) i;
	}
	printf("%x\n", sizeof(uint16_t));
	uint8_t *mem2 = (uint8_t *) 0x100000;
	for (int i = 0; i < 20; i++) {
		printf("%x ", (uint8_t) mem2[i]);
	}

	serial_read_loop();

	return 0;
}

