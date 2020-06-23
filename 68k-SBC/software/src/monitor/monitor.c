
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


extern void init_tty();


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

void info(void)
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


#define TEST_BUF_SIZE 	32
uint16_t test_buffer[TEST_BUF_SIZE] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };

void ramtest()
{
	uint16_t data;

	uint16_t *ram = (uint16_t *) 0x100000;
	for (int i = 0; i < TEST_BUF_SIZE; i++) {
		*ram++ = test_buffer[i];
	}

	int j = 0;
	while (ram < 0x200000) {
		if (*ram++ == test_buffer[j]) {
			j++;
		}
		else
			j = 0;

		if (j >= 5) {
			printf("Found a match at %x\n", ram);
			j = 0;
		}
	}

	printf("\nComplete");
}


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

	puts("\nWrite complete");
}

void verifyrom(void)
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *arduino = (uint16_t *) 0x000000;
	uint16_t *rom = (uint16_t *) ROM_ADDR;
	for (int i = 0; i < ROM_SIZE; i++) {
		if (rom[i] != arduino[i]) {
			printf("F @ %x\n", i);
		}
	}

	puts("\nVerification complete");
}

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
	uint16_t *mem = (uint16_t *) RAM_ADDR;

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
	void (*entry)() = (void (*)()) RAM_ADDR;
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
		putsn("> ");
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
		else if (!strcmp(args[0], "verifyrom")) {
			verifyrom();
		}
		else if (!strcmp(args[0], "ramtest")) {
			ramtest();
		}
		else if (!strcmp(args[0], "dumpram")) {
			dump(RAM_ADDR, 0x1800);
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

char *led = (char *) 0x201c;

#define ARDUINO_TRACE_ON()	asm volatile("movea.l	#0x2019, %%a0\n" "move.b	#1, (%%a0)" : : : "%a0");
#define ARDUINO_TRACE_OFF()	asm volatile("movea.l	#0x2019, %%a0\n" "move.b	#0, (%%a0)" : : : "%a0");

int main()
{
	//init_heap((void *) 0x101000, 0x1000);

	init_tty();

	ARDUINO_TRACE_OFF();

	//delay(10000);

	puts("\n\nWelcome to the 68k Monitor!\n");

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

