

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <kernel/vfs.h>
#include <kernel/syscall.h>


void delay(short count) {
	while (--count > 0) { }
}

int readline(char *buffer, short max)
{
	for (short i = 0; i < max; i++) {
		//buffer[i] = getchar();
		int ret = read(0, &buffer[i], 1);
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

void print_stack()
{
	uint32_t sp;

	asm("move.l %%sp, %0\n" : "=r" (sp));

	printf("SP: %x\n", sp);
	dump((uint16_t *) sp, 64);
	return;
}

void info()
{
	uint32_t sp;
	uint32_t sv1;
	uint16_t flags;

	asm(
	"move.l	%%sp, %0\n"
	"move.l	(%%sp), %1\n"
	"move.w	%%sr, %2\n"
	: "=r" (sp), "=r" (sv1), "=r" (flags)
	);

	printf("SP: %x\n", sp);
	printf("TOP: %x\n", sv1);
	printf("SR: %x\n", flags);
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

void send_file(const char *name)
{
	int fd;
	uint16_t size;
	uint16_t data;
	uint16_t *mem = (uint16_t *) program_mem;

	printf("Loading file %s\n", name);

	if ((fd = open(name, O_CREAT | O_WRONLY)) < 0) {
		printf("Error opening %s: %d\n", name, fd);
		return;
	}

	size = fetch_word();
	size >>= 1;
	//printf("Expecting %x\n", size);

	for (short i = 0; i < size; i++) {
		data = fetch_word();
		//printf("%x ", data);
		//mem[i] = data;
		write(fd, &data, 2);
	}

	close(fd);

	puts("Load complete");
}

void boot()
{
	void (*entry)() = (void (*)()) program_mem;
	((void (*)()) entry)();
}

void sh_fork()
{
	//int pid = SYSCALL1(SYS_FORK, 0);
	int pid = fork();

	if (pid) {
		printf("The child's pid is %d\n", pid);
	}
	else {
		puts("I AM THE CHILD!");
		exit(0);
	}
}

void ls()
{
	int fd;
	int error;
	struct vdir dir;
	struct vfile *file;
	char *cwd = "/";

	if ((fd = open(cwd, 0)) < 0) {
		printf("Error opening %s: %d\n", cwd, fd);
		return;
	}

	while (1) {
		error = readdir(fd, &dir);
		if (error < 0) {
			printf("Error at readdir %d\n", error);
			return;
		}

		if (error == 0)
			break;
		else
			printf("File: %s\n", dir.name);
	}

	close(fd);
}


void command_exec(char *path)
{
	int pid = fork();

	if (pid) {
		printf("The child's pid is %d\n", pid);
	}
	else {
		puts("I AM THE CHILD!");
		exec(path);
		exit(-1);
	}
}



// In process.c
extern void print_run_queue();


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
		else if (!strcmp(args[0], "send")) {
			if (argc <= 1)
				puts("You need file name");
			else
				send_file(args[1]);
		}
		else if (!strcmp(args[0], "boot")) {
			boot();
		}
		else if (!strcmp(args[0], "queue")) {
			print_run_queue();
		}
		else if (!strcmp(args[0], "writerom")) {
			writerom();
		}
		else if (!strcmp(args[0], "dump")) {
			if (argc <= 1)
				puts("You need an address");
			else {
				short length = 0x40;

				if (argc >= 3)
					length = strtol(args[2], NULL, 16);
				dump((const uint16_t *) strtol(args[1], NULL, 16), length);
			}
		}
		else if (!strcmp(args[0], "exec")) {
			if (argc <= 1)
				puts("You need file name");
			else {
				command_exec(args[1]);
			}
		}
		else if (!strcmp(args[0], "fork")) {
			sh_fork();
		}
		else if (!strcmp(args[0], "ls")) {
			ls();
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

	puts("\n\nThe Pseudo Shell!\n");

	serial_read_loop();

	return 0;
}


