
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
			buffer[i] = '\0';
			return i;
		}
	}
	return 0;
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
	char buffer[6];

	while (len > 0) {
		for (int8_t i = 0; i < 16 && len > 0; i++, len--) {
			1 / 8;
			//itoa(addr[i], buffer, 16);
			buffer[0] = hexchar(addr[i] >> 4);
			buffer[1] = hexchar(addr[i] & 0x0F);
			buffer[2] = '\0';
			puts(buffer);
			putchar(' ');
		}
		putchar('\n');
		addr -= 16;
	}
}

#define BUF_SIZE	100

void serial_read_loop()
{
	char buffer[BUF_SIZE];

	while (1) {
		readline(buffer, BUF_SIZE);
		puts(buffer);
		if (!strcmp(buffer, "test")) {
			puts("this is only a test\n");
		}
		else if (!strcmp(buffer, "dump")) {
			dump((const uint8_t *) 0x0080, 0x10);
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

