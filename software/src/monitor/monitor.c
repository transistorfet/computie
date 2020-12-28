/*
 * Monitor - the program in onboard flash which initially boots the processor, and provides some basic serial transfer commands
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


extern void init_tty();
char *led = (char *) 0x201c;


void delay(int count) {
	while (--count > 0) { asm volatile (""); }
}

int readline(char *buffer, short max)
{
	short i = 0;

	while (i < max) {
		buffer[i] = getchar();
		switch (buffer[i]) {
			case 0x08: {
				if (i >= 1) {
					putchar(0x08);
					putchar(' ');
					putchar(0x08);
					i--;
				}
				break;
			}
			case '\n': {
				putchar(buffer[i]);
				buffer[i] = '\0';
				return i;
			}
			default: {
				putchar(buffer[i++]);
				break;
			}
		}
	}
	return i;
}

int parseline(char *input, char **vargs)
{
	short j = 0;

	while (*input == ' ')
		input++;

	vargs[j++] = input;
	while (*input != '\0' && *input != '\n' && *input != '\r') {
		if (*input == ' ') {
			*input = '\0';
			input++;
			while (*input == ' ')
				input++;
			vargs[j++] = input;
		}
		else
 			input++;
	}

	*input = '\0';
	if (*vargs[j - 1] == '\0')
		j -= 1;
	vargs[j] = NULL;

	return j;
}

char hexchar(uint8_t byte)
{
	if (byte < 10)
		return byte + 0x30;
	else
		return byte + 0x37;
}

void dump(const uint16_t *addr, short len)
{
	char buffer[6];

	buffer[4] = ' ';
	buffer[5] = '\0';
	while (len > 0) {
		printf("%x: ", addr);
		for (short i = 0; i < 8 && len > 0; i++, len--) {
			buffer[0] = hexchar((addr[i] >> 12) & 0xF);
			buffer[1] = hexchar((addr[i] >> 8) & 0xF);
			buffer[2] = hexchar((addr[i] >> 4) & 0xF);
			buffer[3] = hexchar(addr[i] & 0x0F);
			putsn(buffer);
		}
		putchar('\n');
		addr += 8;
	}
	putchar('\n');
}



/************
 * Commands *
 ************/

#define ROM_ADDR	0x000000
#define ROM_SIZE	0x1200

#define RAM_ADDR	0x100000
#define RAM_SIZE	1024

#define SUPERVISOR_ADDR	0x200000


void command_dump(int argc, char **args)
{
	if (argc <= 1)
		puts("You need an address");
	else {
		short length = 0x40;

		if (argc >= 3)
			length = strtol(args[2], NULL, 16);
		dump((const uint16_t *) strtol(args[1], NULL, 16), length);
	}
}

void command_dumpram(int argc, char **args)
{
	dump((uint16_t *) RAM_ADDR, 0x1800);
}

void command_info(int argc, char **args)
{
	uint32_t pc;
	uint32_t sp;
	uint32_t sv1;
	uint16_t flags;

	asm(
	"move.l	%%sp, %0\n"
	"move.l	(%%sp), %1\n"
	"move.w	%%sr, %2\n"
	"bsr	INFO\n"
	"INFO:\t"
	"move.l	(%%sp)+, %3\n"
	: "=r" (sp), "=r" (sv1), "=r" (flags), "=r" (pc)
	);

	printf("PC: %x\n", pc);
	printf("SR: %x\n", flags);
	printf("SP: %x\n", sp);
	printf("TOP: %x\n", sv1);
	return;
}

void command_poke(int argc, char **args)
{
	if (argc <= 2)
		puts("You need an address and byte to poke");
	else {
		uint8_t *address = (uint8_t *) strtol(args[1], NULL, 16);
		uint8_t data = (uint8_t) strtol(args[2], NULL, 16);
		*(address) = data;
	}

}

void command_ramtest(int argc, char **args)
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


/*
#define TEST_BUF_SIZE 	32
uint16_t test_buffer[TEST_BUF_SIZE] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };

void command_ramduptest(int argc, char **args)
{
	uint16_t data;

	uint16_t *ram = (uint16_t *) 0x100000;
	for (int i = 0; i < TEST_BUF_SIZE; i++) {
		*ram++ = test_buffer[i];
	}

	int j = 0;
	while ((int) ram < 0x200000) {
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


void command_readrom(int argc, char **args)
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


void command_runtest(int argc, char **args)
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

void erase_flash(int sector)
{
	sector <<= 18;

	puts("Erasing flash sector 0");
	*((volatile uint16_t *) (0x555 << 1)) = 0xAAAA;
	putchar('.');
	*((volatile uint16_t *) (0x2AA << 1)) = 0x5555;
	putchar('.');
	*((volatile uint16_t *) (0x555 << 1)) = 0x8080;
	putchar('.');
	*((volatile uint16_t *) (0x555 << 1)) = 0xAAAA;
	putchar('.');
	*((volatile uint16_t *) (0x2AA << 1)) = 0x5555;
	putchar('.');
	*((volatile uint16_t *) sector) = 0x3030;
	putchar('.');
}

void command_eraserom(int argc, char **args)
{
	uint16_t data;
	uint16_t *dest = (uint16_t *) ROM_ADDR;

	erase_flash(0);
	delay(200000);
	data = dest[0];

	puts("\nVerifying erase\n");
	for (int i = 0; i < ROM_SIZE; i++) {
		data = dest[i];
		if (data != 0xFFFF) {
			printf("Flash not erased at %x (%x)\n", dest + i, data);
			return;
		}
	}

	puts("Rom erased! Make sure to writerom before resetting\n");
}


void program_flash_data(uint16_t *addr, uint16_t data)
{
	*((volatile uint16_t *) (0x555 << 1)) = 0xAAAA;
	*((volatile uint16_t *) (0x2AA << 1)) = 0x5555;
	*((volatile uint16_t *) (0x555 << 1)) = 0xA0A0;
	*((volatile uint16_t *) addr) = data;
}

void command_writerom(int argc, char **args)
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *dest = (uint16_t *) ROM_ADDR;
	uint16_t *source = (uint16_t *) RAM_ADDR;

	for (int i = 0; i < ROM_SIZE; i++) {
		data = dest[i];
		if (data != 0xFFFF) {
			printf("Flash not erased at %x (%x)\n", dest + i, data);
			return;
		}
	}

	for (int i = 0; i < ROM_SIZE; i++) {
		program_flash_data(&dest[i], source[i]);
		for (char j = 0; j < 100; j++) { asm volatile(""); }
		printf("%x ", dest[i]);
	}

	puts("\nWrite complete");
}

void command_verifyrom(int argc, char **args)
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *source = (uint16_t *) RAM_ADDR;
	uint16_t *dest = (uint16_t *) ROM_ADDR;
	for (int i = 0; i < ROM_SIZE; i++) {
		if (dest[i] != source[i]) {
			printf("@%x expected %x but found %x\n", &dest[i], source[i], dest[i]);
			if (++errors > 100) {
				puts("Bailing out\n");
				break;
			}
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

void command_load(int argc, char **args)
{
	uint32_t size;
	uint16_t data;
	uint16_t *mem = (uint16_t *) RAM_ADDR;

	size = fetch_word();
	size >>= 1;
	//printf("Expecting %x\n", size);

	for (int i = 0; i < size; i++) {
		data = fetch_word();
		//printf("%x ", data);
		mem[i] = data;
	}

	puts("Load complete");
}

void command_boot(int argc, char **args)
{
	//*led = 0x0;
	void (*entry)() = (void (*)()) RAM_ADDR;
	((void (*)()) entry)();
}


/**************************
 * Command Line Execution *
 **************************/

struct command {
	char *name;
	void (*func)(int, char **);
};

#define add_command(n, f)	{			\
	command_list[num_commands].name = (n);		\
	command_list[num_commands++].func = (f);	\
}

int load_commands(struct command *command_list)
{
	int num_commands = 0;

	add_command("info", command_info);
	add_command("load", command_load);
	add_command("boot", command_boot);
	add_command("dump", command_dump);
	add_command("poke", command_poke);
	add_command("dumpram", command_dumpram);
	add_command("eraserom", command_eraserom);
	add_command("writerom", command_writerom);
	add_command("verifyrom", command_verifyrom);
	add_command("ramtest", command_ramtest);
	//add_command("ramduptest", command_ramtest);
	//add_command("readrom", command_readrom);
	//add_command("runtest", command_runtest);

	return num_commands;
}

#define BUF_SIZE	100
#define ARG_SIZE	10

void serial_read_loop()
{
	int i;
	short argc;
	char buffer[BUF_SIZE];
	char *args[ARG_SIZE];

	struct command command_list[10];
	int num_commands = load_commands(command_list);

	while (1) {
		putsn("> ");
		readline(buffer, BUF_SIZE);
		argc = parseline(buffer, args);

		if (!strcmp(args[0], "test")) {
			puts("this is only a test");
		}
		else if (!strcmp(args[0], "help")) {
			for (int i = 0; i < num_commands; i++)
				puts(command_list[i].name);
		}
		else {
			for (i = 0; i < num_commands; i++) {
				if (!strcmp(args[0], command_list[i].name)) {
					command_list[i].func(argc, args);
					break;
				}
			}

			if (i >= num_commands && args[0][0] != '\0') {
				puts("Unknown command\n");
			}
		}
	}
}


#define ARDUINO_TRACE_ON()	asm volatile("movea.l	#0x2019, %%a0\n" "move.b	#1, (%%a0)" : : : "%a0");
#define ARDUINO_TRACE_OFF()	asm volatile("movea.l	#0x2019, %%a0\n" "move.b	#0, (%%a0)" : : : "%a0");

int main()
{
	//init_heap((void *) 0x101000, 0x1000);

	//*led = 0x1;

	init_tty();

	//ARDUINO_TRACE_OFF();

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

