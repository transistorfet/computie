

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/syscall.h>


#define RAM_ADDR	0x100000
#define RAM_SIZE	1024

#define ROM_ADDR	0x200000
#define ROM_SIZE	0x1400


void delay(int count) {
	while (--count > 0) { asm volatile(""); }
}

char hexchar(uint8_t byte)
{
	if (byte < 10)
		return byte + 0x30;
	else
		return byte + 0x37;
}

void dump_line(const uint16_t *addr, short len)
{
	char buffer[6];

	buffer[4] = ' ';
	buffer[5] = '\0';

	for (short i = 0; i < 8 && len > 0; i++, len--) {
		buffer[0] = hexchar((addr[i] >> 12) & 0xF);
		buffer[1] = hexchar((addr[i] >> 8) & 0xF);
		buffer[2] = hexchar((addr[i] >> 4) & 0xF);
		buffer[3] = hexchar(addr[i] & 0x0F);
		putsn(buffer);
	}
	putchar('\n');
}

void dump(const uint16_t *addr, short len)
{
	while (len > 0) {
		printf("%06x: ", addr);
		dump_line(addr, len > 8 ? 8 : len);
		addr += 8;
		len -= 8;
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


/************
 * Commands *
 ************/

int command_test(int argc, char **argv)
{
	puts("this is only a test");
}

int command_dump(int argc, char **argv)
{
	if (argc <= 1)
		puts("You need an address");
	else {
		short length = 0x40;

		if (argc >= 3)
			length = strtol(argv[2], NULL, 16);
		dump((const uint16_t *) strtol(argv[1], NULL, 16), length);
	}
	return 0;
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


int command_send(int argc, char **argv)
{
	int fd;
	uint16_t size;
	uint16_t data;

	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	printf("Loading file %s\n", argv[1]);

	if ((fd = open(argv[1], O_CREAT | O_WRONLY, 0755)) < 0) {
		printf("Error opening %s: %d\n", argv[1], fd);
		return fd;
	}

	size = fetch_word();
	size >>= 1;
	//printf("Expecting %x\n", size);

	for (short i = 0; i < size; i++) {
		data = fetch_word();
		//printf("%x ", data);
		//mem[i] = data;
		write(fd, (char *) &data, 2);
	}

	close(fd);

	puts("Load complete");

	return 0;
}

int command_echo(int argc, char **argv)
{
	printf("%s\n", argv[1]);
	return 0;
}


#define DUMP_BUF_SIZE	0x10

int command_hex(int argc, char **argv)
{
	int fd;
	int result;
	int pos = 0;
	char buffer[DUMP_BUF_SIZE];

	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
		printf("Error opening %s: %d\n", argv[1], fd);
		return fd;
	}


	while (1) {
		result = read(fd, buffer, DUMP_BUF_SIZE);
		if (result == 0)
			break;

		if (result < 0) {
			printf("Error while reading: %d\n", result);
			return result;
		}

		printf("%04x: ", pos);
		pos += result;
		dump_line((uint16_t *) buffer, result >> 1);

	}

	putchar('\n');

	close(fd);

	return 0;
}

int command_cat(int argc, char **argv)
{
	int fd;
	int result;
	char buffer[DUMP_BUF_SIZE];

	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
		printf("Error opening %s: %d\n", argv[1], fd);
		return fd;
	}

	while (1) {
		result = read(fd, buffer, DUMP_BUF_SIZE);
		if (result == 0)
			break;

		if (result < 0) {
			printf("Error while reading: %d\n", result);
			return result;
		}

		write(STDOUT_FILENO, buffer, result);
	}

	putchar('\n');

	close(fd);

	return 0;
}

const char *file_permissions = "-rwxrwxrwx";

void format_file_mode(mode_t mode, char *buffer)
{
	mode_t curbit = 0400;

	strcpy(buffer, file_permissions);

	if (mode & S_IFDIR)
		buffer[0] = 'd';

	for (char i = 1; i <= 10; i++) {
		if (!(mode & curbit))
			buffer[i] = '-';
		curbit >>= 1;
	}
}

int command_ls(int argc, char **argv)
{
	int fd;
	int error;
	struct dirent dir;
	struct stat statbuf;
	char filename[100];
	char filemode[10];

	char *path = argc > 1 ? argv[1] : ".";

	if ((fd = open(path, 0, 0)) < 0) {
		printf("Error opening %s: %d\n", path, fd);
		return fd;
	}

	int start = strlen(path) - 1;
	strcpy(filename, path);
	if (filename[start] != '/')
		filename[++start] = '/';
	start++;

	while (1) {
		error = readdir(fd, &dir);
		if (error < 0) {
			printf("Error at readdir %d\n", error);
			return error;
		}

		if (error == 0)
			break;

		if (dir.name[0] != '.') {
			strcpy(&filename[start], dir.name);
			error = stat(filename, &statbuf);
			if (error < 0) {
				printf("Error at stat %s (%d)\n", filename, error);
				return error;
			}

			format_file_mode(statbuf.st_mode, filemode);
			printf("%s %6d %s\n", filemode, statbuf.st_size, dir.name);
		}
	}

	close(fd);

	return 0;
}

int command_mkdir(int argc, char **argv)
{
	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	int error = mkdir(argv[1], 0755);
	if (error < 0) {
		printf("Error while making directory %s: %d\n", argv[1], error);
	}

	return 0;
}

int command_rm(int argc, char **argv)
{
	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	int error = unlink(argv[1]);
	if (error < 0) {
		printf("Error while unlinking %s: %d\n", argv[1], error);
	}

	return 0;
}

int command_chdir(int argc, char **argv)
{
	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	int error = chdir(argv[1]);
	if (error < 0) {
		printf("Error while changing dir %s: %d\n", argv[1], error);
	}

	return 0;
}




int command_exec(int argc, char **argv)
{
	int pid, status;
	char *argv2[2] = { "an arg", NULL };
	char *envp[2] = { NULL };

	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

 	pid = fork();
	if (pid) {
		printf("The child's pid is %d\n", pid);
		waitpid(pid, &status, 0);
		printf("The child exited with %d\n", status);
	}
	else {
		status = exec(argv[1], argv2, envp);
		// The exec() system call will only return if an error occurs
		printf("Failed to execute %s: %d\n", argv[1], status);
		exit(-1);
	}

	return 0;
}




/*****************************
 * Command Input And Parsing *
 *****************************/


int readline(char *buffer, short max)
{
	short i = 0;

	while (i < max) {
		//buffer[i] = getchar();
		int ret = read(0, &buffer[i], 1);
		if (ret != 1) {
			printf("Input error: %d\n", ret);
			return 0;
		}

		switch (buffer[i]) {
			case 0x03:	// Control-C
				return 0;
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
	vargs[j] = NULL;

	return j;
}

struct pipe_command {
	char *cmd;
	char *stdin_file;
	char *stdout_file;
	int append;
};

static inline int is_whitespace(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

char *parse_word(char **input)
{
	char *start;

	for (; **input != '\0' && is_whitespace(**input); (*input)++) { }

	start = *input;

	for (; **input != '\0' && !is_whitespace(**input); (*input)++) { }
	**input = '\0';
	*input++;

	return start;
}

int parse_command_line(char *input, struct pipe_command *commands)
{
	short j = 0;

	commands[j].cmd = input;

	for (; *input != '\0' && *input != '\n' && *input != '\r'; input++) {
		if (*input == '|') {
			*input = '\0';
			j += 1;
			commands[j].cmd = input + 1;
		}
		else if (*input == '>') {
			if (*(input + 1) == '>') {
				commands[j].append = 1;
				input++;
			}
			*input++ = '\0';

			if (commands[j].stdout_file) {
				printf("parse error: stdout is redirected more than once\n");
				return -1;
			}
			commands[j].stdout_file = parse_word(&input);
		}
		else if (*input == '<') {
			*input++ = '\0';
			if (commands[j].stdin_file) {
				printf("parse error: stdin is redirected more than once\n");
				return -1;
			}
			commands[j].stdin_file = parse_word(&input);
		}
	}

	return 0;
}

extern void test_pipe();
extern void test_forkpipe();

typedef int (*main_t)(int argc, char **argv);

struct command {
	char *name;
	main_t main;
};

struct command command_list[] = {
	{ "test", 	command_test },
	{ "dump", 	command_dump },
	{ "send", 	command_send },
	{ "echo", 	command_echo },
	{ "hex", 	command_hex },
	{ "cat", 	command_cat },
	{ "ls", 	command_ls },
	{ "mkdir", 	command_mkdir },
	{ "rm", 	command_rm },
	{ "cd", 	command_chdir },
	{ "exec", 	command_exec },
	{ "test_pipe", 	test_pipe },
	{ "test_forkpipe", 	test_forkpipe },
	{ NULL },
};

main_t find_command(char *name)
{
	for (short i = 0; command_list[i].name; i++) {
		if (!strcmp(name, command_list[i].name))
			return command_list[i].main;
	}
	return NULL;
}

int open_file(char *filename, int flags, int newfd)
{
	int fd;

	fd = open(filename, flags, 0644);
	if (fd < 0) {
		printf("Error opening file %s: %d\n", filename, fd);
		return -1;
	}

	if (dup2(fd, newfd))
		return -1;
	return 0;
}

int execute_command(void *func, struct pipe_command *command, int argc, char **argv)
{
	int pid, status;
	char *envp[2] = { NULL };

 	pid = fork();
	if (pid) {
		waitpid(pid, &status, 0);
	}
	else {
		if (command->stdout_file) {
			if (open_file(command->stdout_file, O_WRONLY | O_CREAT | (command->append ? O_APPEND : O_TRUNC), STDOUT_FILENO))
				exit(-1);
		}

		//status = exec(argv[1], argv2, envp);
		status = SYSCALL3(SYS_EXECBUILTIN, (int) func, (int) argv, (int) envp);
		// The exec() system call will only return if an error occurs
		printf("Failed to execute %s: %d\n", argv[1], status);
		exit(-1);
	}

	return 0;
}


#define BUF_SIZE	100
#define ARG_SIZE	10
#define PIPE_SIZE	5

void serial_read_loop()
{
	int argc;
	main_t main;
	char buffer[BUF_SIZE];
	char *argv[ARG_SIZE];
	struct pipe_command commands[PIPE_SIZE];

	while (1) {
		memset_s(commands, 0, sizeof(struct pipe_command) * PIPE_SIZE);
		putsn("% ");
		if (!readline(buffer, BUF_SIZE)) {
			putchar('\n');
			continue;
		}
		if (parse_command_line(buffer, commands))
			continue;

		/*
		for (int i = 0; i < PIPE_SIZE && commands[i].cmd; i++) {
			printf("C: %s\n", commands[i].cmd);
			printf("I: %s\n", commands[i].stdin_file);
			printf("O: %s\n", commands[i].stdout_file);
		}
		*/

		argc = parseline(commands[0].cmd, argv);

		if (*argv[0] == '\0')
			continue;
		else if (!strcmp(argv[0], "cd")) {
			command_chdir(argc, argv);
			continue;
		}

		// TODO this is a hack that should be removed
		if (!strcmp(argv[0], "sync")) {
			minix_sync(NULL);
			continue;
		}

		main = find_command(argv[0]);
		if (main)
			execute_command(main, &commands[0], argc, argv);
		else
			puts("Command not found");

	}
}


extern int tty_68681_tx_safe_mode();

int test_files();
int test_dirs();

/**************
 * Main Entry *
 **************/

int sh_task()
{
	//test_files();
	//test_dirs();


	puts("\n\nThe Pseudo Shell!\n");

	serial_read_loop();

	tty_68681_tx_safe_mode();
	puts("Exiting to monitor");
	return 0;
}


