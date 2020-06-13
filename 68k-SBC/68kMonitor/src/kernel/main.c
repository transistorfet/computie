
#include "tty.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/filedesc.h>
#include <kernel/syscall.h>


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

void load(void)
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

void boot(void)
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

extern struct inode *tty_inode;
static fd_table_t fd_table;

int main()
{
	//load_data_segment();
	init_heap((void *) 0x110000, 0x1000);
	init_interrupts();
	init_syscall();

	init_inode();
	init_fd_table(fd_table);
	init_tty();


	// Allocate memory for loading a processess
	program_mem = malloc(0x1000);


	int fd = new_fd(fd_table, tty_inode);


	struct file *f = get_fd(fd_table, fd);
	//if (!f)
	//	return 0;
	dev_write(f->inode->device, "\nTEST\n", 6);




	puts("\n\nWelcome to the \x1b[32mOS!\n");
	//dev_write(0, "\n\nWelcome to the \x1b[32mthing!\n", 29);

	// Force an address error
	//uint16_t *data = (uint16_t *) 0x100001;
	//uint16_t value = *data;

	//char *str = "Hey syscall, %d";
	//asm(
	//"move.l	#2, %%d0\n"
	//"move.l	%0, %%d1\n"
	//"move.l	#125, %%a0\n"
	//"trap	#1\n"
	//: : "r" (str)
	//);

	SYSCALL(2, "Hey There, %d\n", 1337, 0);

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

