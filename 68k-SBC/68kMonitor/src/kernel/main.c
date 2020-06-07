
#include "tty.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


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
		addr -= 16;
	}
	putchar('\n');
}

void info(void)
{
	uint16_t sp;
	uint16_t sv1;

	asm(
	"move.w	%%a7, %0\n"
	"move.w	(%%a7), %1\n"
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

/*
void ramtest(void)
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *mem = (uint16_t *) RAM_ADDR;
	for (int i = 0; i < RAM_SIZE; i++) {
		mem[i] = (uint16_t) i;
	}

	
	//uint8_t *mem2 = (uint8_t *) RAM_ADDR;
	//for (int i = 0; i < RAM_SIZE; i++) {
	//	printf("%x ", (uint8_t) mem2[i]);
	//}


	for (int i = 0; i < RAM_SIZE; i++) {
		data = (uint16_t) mem[i];
		printf("%x ", data);
		if (data != i)
			errors++;
	}

	printf("\nErrors: %d", errors);
}


void readrom(void)
{
	uint16_t data;
	uint16_t reference;
	uint16_t errors = 0;

	uint16_t *arduino = (uint16_t *) 0x000000;
	uint16_t *rom = (uint16_t *) ROM_ADDR;
	for (int i = 0; i < ROM_SIZE; i++) {
		data = (uint16_t) rom[i];
		reference = arduino[i];
		printf("%x %x\n", data, reference);
		if (data != reference)
			errors++;
	}

	printf("\nErrors: %d", errors);
}


void run_rom_from_ram(void)
{
	uint16_t *arduino = (uint16_t *) 0x000000;
	uint16_t *mem = (uint16_t *) RAM_ADDR;
	for (int i = 0; i < 4200; i++) {
		mem[i] = arduino[i];
	}

	void (*entry)() = (void (*)()) (RAM_ADDR + 0x20);
	((void (*)()) entry)();
}
*/

void writerom(void)
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
/*
		else if (!strcmp(args[0], "ramtest")) {
			ramtest();
		}
		else if (!strcmp(args[0], "readrom")) {
			readrom();
		}
		else if (!strcmp(args[0], "runtest")) {
			run_rom_from_ram();
		}
*/
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
	}
}

/*
const char data_segment[] = { 0x00, 0x00, 0x20, 0x1c, 0x00, 0x70, 0x00, 0x07, 0x00, 0x70, 0x00, 0x07, 0x00, 0x70, 0x00, 0x03 };

void load_data_segment()
{
	char *ram = (char *) RAM_ADDR;
	for (uint16_t i = 0; i < 18; i++) {
		ram[i] = data_segment[i];
	}
}
*/

#include <kernel/filedesc.h>
extern struct inode *tty_inode;
static fd_table_t fd_table;

int main()
{
	//load_data_segment();
	init_heap((void *) 0x110000, 0x1000);

	init_inode();
	init_fd_table(fd_table);
	init_tty();

	int fd = new_fd(fd_table, tty_inode);

	//delay(10000);

	puts("\n\nWelcome to the \x1b[32mOS!\n");
	//dev_write(0, "\n\nWelcome to the \x1b[32mthing!\n", 29);

	/*
	int *data = malloc(sizeof(int) * 10);
	data[0] = 0;
	data[1] = 1;
	data[2] = 2;
	printf("%x %x %x\n", &data[0], &data[1], &data[2]);
	printf("%d\n", *(data - 3));
	free(data);

	int *data2 = malloc(4);
	printf("%x %d\n", data2, *(data2 - 3));

	int *data3 = malloc(sizeof(int) * 2);
	printf("%x %d\n", data3, *(data3 - 3));

	free(data2);
	//printf("%x %d\n", data2, *(data2 - 3));
	print_free();

	int *data4 = malloc(sizeof(int) * 10);
	printf("%x %d\n", data4, *(data4 - 3));
	
	print_free();
	*/

	/*
	uint8_t *mem2 = (uint8_t *) 0x100000;
	for (short i = 0; i < 20; i++) {
		printf("%x ", (uint8_t) mem2[i]);
	}
	*/

	/*
	uint16_t *mem3 = (uint16_t *) 0x200000;
	for (int i = 0; i < 0x100; i++) {
		printf("%x ", (uint16_t) mem3[i]);
	}
	*/

	/*
	// Cause an address exception
	char test[10];
	*((short *) &test[1]) = 10;
	*/

	serial_read_loop();

	return 0;
}

