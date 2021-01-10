
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <kernel/syscall.h>

#include "prototype.h"

#ifdef LINUXHOST
#define putsn(x) { fputs((x), stdout); fflush(stdout); }
#endif

#ifdef ADDMAIN
int sh_task();

int main()
{
	return sh_task();
}
#endif


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


/************
 * Commands *
 ************/

int command_dump(int argc, char **argv, char **envp)
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

int command_poke(int argc, char **args, char **envp)
{
	if (argc <= 2)
		puts("You need an address and byte to poke");
	else {
		uint8_t *address = (uint8_t *) strtol(args[1], NULL, 16);
		uint8_t data = (uint8_t) strtol(args[2], NULL, 16);
		*(address) = data;
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


int command_send(int argc, char **argv, char **envp)
{
	int fd;
	uint16_t size;
	uint16_t data;
	tcflag_t lflag;
	struct termios tio;

	if (argc <= 1) {
		puts("You need file name");
		return -1;
	}

	tcgetattr(STDIN_FILENO, &tio);
	lflag = tio.c_lflag;
	tio.c_lflag = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &tio);
	tio.c_lflag = lflag;

	printf("Loading file %s\n", argv[1]);

	if ((fd = open(argv[1], O_CREAT | O_WRONLY, 0755)) < 0) {
		printf("Error opening %s: %d\n", argv[1], fd);
		tcsetattr(STDIN_FILENO, TCSANOW, &tio);
		return fd;
	}

	size = fetch_word();
	size >>= 1;
	printf("Expecting %x\n", size);

	for (short i = 0; i < size; i++) {
		data = fetch_word();
		//printf("%x ", data);
		//mem[i] = data;
		write(fd, (char *) &data, 2);
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &tio);

	close(fd);

	puts("Load complete");

	return 0;
}


#define DUMP_BUF_SIZE	0x10

int command_hex(int argc, char **argv, char **envp)
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


int command_chdir(int argc, char **argv, char **envp)
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


#ifdef ONEBINARY
#include "echo.c"
#include "cat.c"
#include "ls.c"
#include "mkdir.c"
#include "mv.c"
#include "cp.c"
#include "ln.c"
#include "rm.c"
#include "chmod.c"
#include "date.c"
#include "ps.c"
#include "kill.c"
#include "mount.c"
#include "umount.c"
#include "sync.c"
#endif



/*****************************
 * Command Input And Parsing *
 *****************************/

struct pipe_command {
	char *cmd;
	char *stdin_file;
	char *stdout_file;
	int append;
};


typedef int (*main_t)(int argc, char **argv, char **envp);

struct command {
	char *name;
	main_t main;
};

int commands;
struct command command_list[20];

#define add_command(n, f)	{		\
	command_list[commands].name = (n);	\
	command_list[commands++].main = (f);	\
}

void init_commands()
{
	commands = 0;

	add_command("dump", 	command_dump);
	add_command("poke", 	command_poke);
	add_command("send", 	command_send);
	add_command("hex", 	command_hex);
	add_command("cd", 	command_chdir);

	#ifdef ONEBINARY
	add_command("echo", 	command_echo);
	add_command("cat", 	command_cat);
	add_command("ls", 	command_ls);
	add_command("mkdir", 	command_mkdir);
	add_command("cp", 	command_cp);
	add_command("mv", 	command_mv);
	add_command("ln", 	command_ln);
	add_command("rm", 	command_rm);
	add_command("chmod", 	command_chmod);
	add_command("date", 	command_date);
	add_command("ps", 	command_ps);
	add_command("kill", 	command_kill);
	add_command("mount", 	command_mount);
	add_command("umount", 	command_umount);
	add_command("sync", 	command_sync);
	#endif

	add_command(NULL, 	NULL);
}

main_t find_command(char *name)
{
	for (short i = 0; command_list[i].name; i++) {
		if (!strcmp(name, command_list[i].name))
			return command_list[i].main;
	}
	return NULL;
}

/*
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
	buffer[--i] = '\0';
	return i;
}
*/

int parseline(char *input, char **vargs)
{
	short j = 0;

	while (*input == ' ' || *input == '\t')
		input++;

	vargs[j++] = input;
	while (*input != '\0' && *input != '\n' && *input != '\r') {
		if (*input == '\"') {
			if (vargs[j - 1] != input) {
				*input++ = '\0';
				vargs[j++] = input;
			}
			input++;

			while (!iscntrl(*input) && *input != '\"')
				input++;
			*input++ = '\0';
			while (*input == ' ' || *input == '\t')
				input++;
			vargs[j++] = input;
		}
		else if (*input == ' ' || *input == '\t') {
			*input++ = '\0';
			while (*input == ' ' || *input == '\t')
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

char *parse_word(char **input)
{
	char *start;

	for (; **input != '\0' && isspace(**input); (*input)++) { }

	start = *input;

	for (; **input != '\0' && !isspace(**input); (*input)++) { }
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
			if (*(input + 1) == '>')
				commands[j].append = 1;
			*input++ = '\0';
			for (; *input == ' ' || *input == '>'; input++);

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

int open_file(char *filename, int flags, int newfd)
{
	int fd;

	fd = open(filename, flags, 0666);
	if (fd < 0) {
		printf("Error opening file %s: %d\n", filename, fd);
		return -1;
	}

	if (dup2(fd, newfd))
		return -1;
	return 0;
}

char *resolve_file_location(char *filename, char *buffer, int max)
{
	// TODO replace with the environment variable
	const char *PATH = "/bin:/sbin";

	if (strchr(filename, '/')) {
		if (!access(filename, X_OK))
			return filename;
		return NULL;
	}

	short i;
	const char *curpath = PATH;
	while (*curpath != '\0') {
		for (i = 0; i < max - 1 && *curpath && *curpath != ':'; curpath++, i++)
			buffer[i] = *curpath;
		if (*curpath == ':')
			curpath++;
		buffer[i++] = '/';
		strncpy(&buffer[i], filename, max - i);
		buffer[max - 1] = '\0';

		if (!access(buffer, X_OK))
			return buffer;
	}
	return NULL;
}

#define NAME_SIZE	100

int execute_command(struct pipe_command *command, int argc, char **argv, char **envp)
{
	int pid, status;
	char *fullpath;
	char buffer[NAME_SIZE];
	main_t main = NULL;

	fullpath = resolve_file_location(argv[0], buffer, NAME_SIZE);
	if (fullpath)
		argv[0] = fullpath;
	else {
		main = find_command(argv[0]);
		if (!main) {
			puts("Command not found");
			return 1;
		}
	}

 	pid = fork();
	if (pid) {
		waitpid(pid, &status, 0);
		pid_t fgpid = getpgid(0);
		tcsetpgrp(STDOUT_FILENO, fgpid);
	}
	else {
		if (setpgid(0, 0))
			exit(-1);
		pid_t fgpid = getpgid(0);
		tcsetpgrp(STDOUT_FILENO, fgpid);

		// TODO set the tty's process group to this one? how does it get set back when the process terminates?

		if (command->stdout_file) {
			if (open_file(command->stdout_file, O_WRONLY | O_CREAT | (command->append ? O_APPEND : O_TRUNC), STDOUT_FILENO))
				exit(-1);
		}

		if (main) {
			#ifndef LINUXHOST
			status = SYSCALL3(SYS_EXECBUILTIN, (int) main, (int) argv, (int) envp);
			#else
			status = main(argc, argv, envp);
			#endif
		}
		else
			status = execve(argv[0], argv, envp);
		// The exec() system call will only return if an error occurs
		printf("Failed to execute %s: %d\n", argv[1], status);
		exit(-1);
	}

	return 0;
}


#define BUF_SIZE	256
#define ARG_SIZE	10
#define PIPE_SIZE	5

void serial_read_loop()
{
	int argc;
	int error;
	main_t main;
	char buffer[BUF_SIZE];
	char *argv[ARG_SIZE];
	struct pipe_command commands[PIPE_SIZE];
	char *envp[2] = { "PATH=/bin:/sbin", NULL };

	while (1) {
		memset(commands, 0, sizeof(struct pipe_command) * PIPE_SIZE);
		putsn("% ");
		//if (!readline(buffer, BUF_SIZE))
		if ((error = read(0, buffer, BUF_SIZE)) <= 0) {
			if (error != EINTR)
				printf("Error: %d\n", error);
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

		if (argc <= 0 || !argv[0] || *argv[0] == '\0')
			continue;
		else if (!strcmp(argv[0], "exit"))
			return;
		else if (!strcmp(argv[0], "cd")) {
			command_chdir(argc, argv, envp);
			continue;
		}

		execute_command(&commands[0], argc, argv, envp);
	}
}

void handle_signal(int signum)
{
	if (signum == SIGINT)
		puts("^C");
}

/**************
 * Main Entry *
 **************/

//int sh_task()
int MAIN(sh_task)()
{
	init_commands();

	puts("\n\nThe Pseudo Shell!\n");

	struct sigaction act;
	act.sa_handler = handle_signal;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGALRM, &act, NULL);

	//alarm(3);
	//pause();
	//puts("Hey!\n");

	//test_math();

	serial_read_loop();

	return 0;
}


