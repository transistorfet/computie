/*
 * Monitor - the program in onboard flash which initially boots the processor, and provides some basic serial transfer commands
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


extern void init_tty();
char *led = (char *) 0x201c;


void delay(int count)
{
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
			fputs(buffer, stdout);
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
#define ROM_SIZE	0x1800

#define RAM_ADDR	0x100000
#define RAM_SIZE	1024

#define SUPERVISOR_ADDR	0x200000


void command_dump(int argc, char **args)
{
	if (argc <= 1)
		fputs("You need an address\n", stdout);
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
		fputs("You need an address and byte to poke\n", stdout);
	else {
		uint8_t *address = (uint8_t *) strtol(args[1], NULL, 16);
		uint8_t data = (uint8_t) strtol(args[2], NULL, 16);
		*(address) = data;
	}

}

void erase_flash(uint32_t sector)
{
	#ifdef BOARD_K30
	printf("Erasing flash sector %d", sector);
	*((volatile uint8_t *) 0x555) = 0xAA;
	putchar('.');
	*((volatile uint8_t *) 0x2AA) = 0x55;
	putchar('.');
	*((volatile uint8_t *) 0x555) = 0x80;
	putchar('.');
	*((volatile uint8_t *) 0x555) = 0xAA;
	putchar('.');
	*((volatile uint8_t *) 0x2AA) = 0x55;
	putchar('.');
	*((volatile uint8_t *) sector) = 0x30;
	putchar('.');
	#else
	printf("Erasing flash sector %d", sector);
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
	#endif
}

#ifdef BOARD_K30
#define SECTOR_SIZE	0x010000
#else
#define SECTOR_SIZE	0x020000
#endif

void command_eraserom(int argc, char **args)
{
	uint16_t data;
	uint16_t *dest = (uint16_t *) ROM_ADDR;
	uint32_t sector = 0;

	if (argc >= 2) {
		sector = strtol(args[1], NULL, 16);
		if ((sector & (SECTOR_SIZE - 1)) || (sector >= RAM_ADDR)) {
			printf("Invalid sector address to erase (%x)\n", sector);
			return;
		}
		dest = (uint16_t *) sector;
	}

	erase_flash(sector);
	delay(300000);
	data = dest[0];

	fputs("\nVerifying erase\n\n", stdout);
	for (int i = 0; i < ROM_SIZE; i++) {
		data = dest[i];
		if (data != 0xFFFF) {
			printf("Flash not erased at %x (%x)\n", dest + i, data);
			return;
		}
	}

	fputs("Rom erased! Make sure to writerom before resetting\n\n", stdout);
}


void program_flash_data(uint16_t *addr, uint16_t data)
{
	#ifdef BOARD_K30
	*((volatile uint8_t *) 0x555) = 0xAA;
	*((volatile uint8_t *) 0x2AA) = 0x55;
	*((volatile uint8_t *) 0x555) = 0xA0;
	*((volatile uint8_t *) addr) = (uint8_t) (data >> 8);
	delay(200);
	*((volatile uint8_t *) 0x555) = 0xAA;
	*((volatile uint8_t *) 0x2AA) = 0x55;
	*((volatile uint8_t *) 0x555) = 0xA0;
	*(((volatile uint8_t *) addr) + 1) = (uint8_t) (data & 0xFF);
	#else
	*((volatile uint16_t *) (0x555 << 1)) = 0xAAAA;
	*((volatile uint16_t *) (0x2AA << 1)) = 0x5555;
	*((volatile uint16_t *) (0x555 << 1)) = 0xA0A0;
	*((volatile uint16_t *) addr) = data;
	#endif
}

void command_writerom(int argc, char **args)
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *dest = (uint16_t *) ROM_ADDR;
	uint16_t *source = (uint16_t *) RAM_ADDR;

	if (argc >= 2)
		dest = (uint16_t *) strtol(args[1], NULL, 16);

	for (int i = 0; i < ROM_SIZE; i++) {
		data = dest[i];
		if (data != 0xFFFF) {
			printf("Flash not erased at %x (%x)\n", dest + i, data);
			return;
		}
	}

	for (int i = 0; i < ROM_SIZE; i++) {
		program_flash_data(&dest[i], source[i]);
		delay(200);
		printf("%x ", dest[i]);
	}

	fputs("\nWrite complete\n", stdout);
}

void command_verifyrom(int argc, char **args)
{
	uint16_t data;
	uint16_t errors = 0;

	uint16_t *source = (uint16_t *) RAM_ADDR;
	uint16_t *dest = (uint16_t *) ROM_ADDR;

	if (argc >= 2)
		dest = (uint16_t *) strtol(args[1], NULL, 16);

	for (int i = 0; i < ROM_SIZE; i++) {
		if (dest[i] != source[i]) {
			printf("@%x expected %x but found %x\n", &dest[i], source[i], dest[i]);
			if (++errors > 100) {
				fputs("Bailing out\n", stdout);
				break;
			}
		}
	}

	fputs("\nVerification complete\n", stdout);
}

uint16_t fetch_word(char max)
{
	char buffer[4];

	for (char i = 0; i < max; i++) {
		buffer[i] = getchar();
		buffer[i] = buffer[i] <= '9' ? buffer[i] - 0x30 : buffer[i] - 0x37;
	}

	return (buffer[0] << 12) | (buffer[1] << 8) | (buffer[2] << 4) | buffer[3];
}

void command_load(int argc, char **args)
{
	int i;
	char odd_size;
	uint32_t size;
	uint16_t data;
	uint16_t *mem = (uint16_t *) RAM_ADDR;

	size = fetch_word(4);
	odd_size = size & 0x01;
	size >>= 1;
	//printf("Expecting %x\n", size);

	if (argc >= 2)
		mem = (uint16_t *) strtol(args[1], NULL, 16);

	for (i = 0; i < size; i++) {
		data = fetch_word(4);
		//printf("%x ", data);
		mem[i] = data;
	}

	if (odd_size)
		mem[i] = fetch_word(2);

	fputs("Load complete\n", stdout);
}

void command_boot(int argc, char **args)
{
	char *boot_args = "";
	void (*entry)(char *) = (void (*)(char *)) RAM_ADDR;

	if (argc >= 2)
		entry = (void (*)(char *)) strtol(args[1], NULL, 16);
	if (argc >= 3)
		boot_args = args[2];
	((void (*)()) entry)(boot_args);
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



#define ATA_REG_DEV_CONTROL	((volatile uint8_t *)  0x60001C)
#define ATA_REG_DEV_ADDRESS	((volatile uint8_t *)  0x60001E)
#define ATA_REG_DATA		((volatile uint16_t *) 0x600020)
#define ATA_REG_DATA_BYTE	((volatile uint8_t *)  0x600020)
#define ATA_REG_FEATURE		((volatile uint8_t *)  0x600022)
#define ATA_REG_ERROR		((volatile uint8_t *)  0x600022)
#define ATA_REG_SECTOR_COUNT	((volatile uint8_t *)  0x600024)
#define ATA_REG_SECTOR_NUM	((volatile uint8_t *)  0x600026)
#define ATA_REG_CYL_LOW		((volatile uint8_t *)  0x600028)
#define ATA_REG_CYL_HIGH	((volatile uint8_t *)  0x60002A)
#define ATA_REG_DRIVE_HEAD	((volatile uint8_t *)  0x60002C)
#define ATA_REG_STATUS		((volatile uint8_t *)  0x60002E)
#define ATA_REG_COMMAND		((volatile uint8_t *)  0x60002E)

#define ATA_CMD_READ_SECTORS	0x20
#define ATA_CMD_WRITE_SECTORS	0x30
#define ATA_CMD_IDENTIFY	0xEC
#define ATA_CMD_SET_FEATURE	0xEF

#define ATA_ST_BUSY		0x80
#define ATA_ST_DATA_READY	0x08
#define ATA_ST_ERROR		0x01

#define ATA_DELAY(x)		{ for (int delay = 0; delay < (x); delay++) { asm volatile(""); } }
#define ATA_WAIT()		{ ATA_DELAY(4); while (*ATA_REG_STATUS & ATA_ST_BUSY) { } }
#define ATA_WAIT_FOR_DATA()	{ while (!(*ATA_REG_STATUS) & ATA_ST_DATA_READY) { } }

void command_cfreset(int argc, char **args)
{
	*ATA_REG_DEV_CONTROL = 0xC0;
	ATA_DELAY(10);
	*ATA_REG_DEV_CONTROL = 0x80;
	ATA_DELAY(1000);

	char status = *ATA_REG_STATUS;
	printf("Status: %x\n", status);

	return 0;
}

int ata_check_busy()
{
	int count = 0;
	char status;

	do {
		ATA_DELAY(100);
		status = *ATA_REG_STATUS;
		printf("Status: %x\n", status);
		count += 1;
		//if (status & ATA_ST_ERROR)
		//	return -1;
	} while ((status & 0x80) && count < 20);

	return 0;
}

void command_cftest(int argc, char **args)
{
	char status;
	char buffer[1024];

	if (ata_check_busy() < 0) {
		printf("error\n");
		return -1;
	}

	ATA_DELAY(10);
	status = *ATA_REG_ERROR;
	printf("Error: %x\n", status);

	// Set 8-bit mode
	(*ATA_REG_FEATURE) = 0x01;
	(*ATA_REG_COMMAND) = ATA_CMD_SET_FEATURE;
	//ATA_WAIT();
	if (ata_check_busy() < 0) {
		printf("error\n");
		return -1;
	}
	printf("Set 8-bit mode\n");

	// Read a sector
	int sector = 0;
	(*ATA_REG_DRIVE_HEAD) = 0xE0;
	//(*ATA_REG_DRIVE_HEAD) = 0xE0 | (uint8_t) ((sector >> 24) & 0x0F);
	(*ATA_REG_CYL_HIGH) = (uint8_t) (sector >> 16);
	(*ATA_REG_CYL_LOW) = (uint8_t) (sector >> 8);
	(*ATA_REG_SECTOR_NUM) = (uint8_t) sector;
	(*ATA_REG_SECTOR_COUNT) = 1;
	(*ATA_REG_COMMAND) = ATA_CMD_READ_SECTORS;
	//ATA_WAIT();

	if (ata_check_busy() < 0) {
		printf("error\n");
		return -1;
	}

	printf("Transfer setup.  Waiting for data...\n");
	//ATA_WAIT();
	ATA_WAIT_FOR_DATA();

	printf("Reading data\n");
	for (int i = 0; i < 512; i++) {
		//((uint16_t *) buffer)[i] = (*ATA_REG_DATA);
		//asm volatile("rol.w	#8, %0\n" : "+g" (((uint16_t *) buffer)[i]));
		buffer[i] = (*ATA_REG_DATA_BYTE);
		ATA_DELAY(10);

		ATA_WAIT();
		ATA_DELAY(10);
	}

	for (int i = 0; i < 512; i++) {
		printf("%x ", buffer[i]);
	}
	printf("\n");

	return 0;
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

	add_command("cfreset", command_cfreset);
	add_command("cftest", command_cftest);
	add_command("ramtest", command_ramtest);

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

	struct command command_list[20];
	int num_commands = load_commands(command_list);

	while (1) {
		fputs("> ", stdout);
		readline(buffer, BUF_SIZE);
		argc = parseline(buffer, args);

		if (!strcmp(args[0], "test")) {
			fputs("this is only a test\n", stdout);
		}
		else if (!strcmp(args[0], "help")) {
			for (int i = 0; i < num_commands; i++) {
				fputs(command_list[i].name, stdout);
				putchar('\n');
			}
		}
		else {
			for (i = 0; i < num_commands; i++) {
				if (!strcmp(args[0], command_list[i].name)) {
					command_list[i].func(argc, args);
					break;
				}
			}

			if (i >= num_commands && args[0][0] != '\0') {
				fputs("Unknown command\n", stdout);
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

	fputs("\n\nWelcome to the 68k Monitor!\n\n", stdout);

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

