
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void delay(short count) {
	while (--count > 0) { }
}

int readline(char *buffer, short max)
{
	for (short i = 0; i < max; i++) {
		buffer[i] = getchar();
		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			return i;
		}
	}
	return 0;
}

int parseline(char *input, char **vargs)
{
	short j = 0;

	while (*input == ' ')
		input++;

	vargs[j++] = input;
	for (; *input != '\0' && *input != '\n' && *input != '\r'; input++) {
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

void dump(const uint8_t *addr, short len)
{
	char buffer[4];

	buffer[2] = ' ';
	buffer[3] = '\0';
	while (len > 0) {
		printf("%x: ", addr);
		for (int8_t i = 0; i < 16 && len > 0; i++, len--) {
			buffer[0] = hexchar(addr[i] >> 4);
			buffer[1] = hexchar(addr[i] & 0x0F);
			putsn(buffer);
		}
		putchar('\n');
		addr += 16;
	}
	putchar('\n');
}

void print_stack()
{
	uint32_t sp;

	asm("move.l %%sp, %0\n" : "=r" (sp));

	printf("SP: %x\n", sp);
	dump(sp, 64);
	return;
}

void print_queue()
{
	print_run_queue();
}

void info()
{
	uint32_t sp;
	uint32_t sv1;

	asm(
	"move.l	%%sp, %0\n"
	"move.l	(%%sp), %1\n"
	: "=r" (sp), "=r" (sv1)
	);

	printf("SP: %x\n", sp);
	printf("TOP: %x\n", sv1);
	return;
}

#define RAM_ADDR	0x100000
#define RAM_SIZE	1024

#define ROM_ADDR	0x200000
#define ROM_SIZE	0x1400

void writerom()
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *arduino = (uint16_t *) 0x000000;
	uint16_t *rom = (uint16_t *) ROM_ADDR;
	for (int i = 0; i < ROM_SIZE; i++) {
		rom[i] = (uint16_t) arduino[i];
		for (char j = 0; j < 100; j++) {}
		printf("%x ", rom[i]);
	}

	printf("\nWrite complete");
}

uint16_t *program_mem = NULL;

uint16_t fetch_word()
{
	char buffer[4];

	for (char i = 0; i < 4; i++) {
		buffer[i] = getchar();
		buffer[i] = buffer[i] <= '9' ? buffer[i] - 0x30 : buffer[i] - 0x37;
	}

	return (buffer[0] << 12) | (buffer[1] << 8) | (buffer[2] << 4) | buffer[3];
}

void load()
{
	uint16_t size;
	uint16_t data;
	uint16_t *mem = (uint16_t *) program_mem;

	size = fetch_word();
	size >>= 1;
	//printf("Expecting %x\n", size);

	for (short i = 0; i < size; i++) {
		data = fetch_word();
		//printf("%x ", data);
		mem[i] = data;
	}

	puts("Load complete");
}

void boot()
{
	void (*entry)() = (void (*)()) program_mem;
	((void (*)()) entry)();
}



#define BUF_SIZE	100
#define ARG_SIZE	10

void serial_read_loop()
{
	int16_t argc;
	char buffer[BUF_SIZE];
	char *args[ARG_SIZE];

	while (1) {
		readline(buffer, BUF_SIZE);
		puts(buffer);
		argc = parseline(buffer, args);

		if (!strcmp(args[0], "test")) {
			puts("this is only a test");
		}
		else if (!strcmp(args[0], "info")) {
			info();
		}
		else if (!strcmp(args[0], "load")) {
			load();
		}
		else if (!strcmp(args[0], "boot")) {
			boot();
		}
		else if (!strcmp(args[0], "queue")) {
			print_queue();
		}
		else if (!strcmp(args[0], "writerom")) {
			writerom();
		}
		else if (!strcmp(args[0], "dump")) {
			if (argc <= 1)
				puts("You need an address");
			else {
				dump((const uint8_t *) strtol(args[1], NULL, 16), 0x10);
			}
		}
		else if (!strcmp(args[0], "exit")) {
			return;
		}
	}
}



int sh_task()
{
	// Allocate memory for loading a processess
	program_mem = malloc(0x1000);

	puts("\n\nWelcome to the \x1b[32mOS!\n");

	serial_read_loop();

	return 0;
}


