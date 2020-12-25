

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/ioc_tty.h>
#include <kernel/syscall.h>

#include "prototype.h"

/*
int sh_task();

int main()
{
	return sh_task();
}
*/

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

int command_test(int argc, char **argv, char **envp)
{
	//puts("this is only a test");
	return 0;
}

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

#define ECHO_BUF_SIZE	256

int command_echo(int argc, char **argv, char **envp)
{
	// TODO this would normally be in _start
	environ = envp;

	int k = 0;
	char tmp;
	char *value;
	int open_quote = 0;
	char buffer[ECHO_BUF_SIZE];

	for (int i = 1; i < argc; i++) {
		for (int j = 0; argv[i][j]; j++) {
			if (argv[i][j] == '\"') {
				open_quote = !open_quote;
			}
			else if (argv[i][j] == '$') {
				int l;
				for (l = ++j; argv[i][l] && isalnum(argv[i][l]); l++) { }
				tmp = argv[i][l];
				argv[i][l] = '\0';
				value = getenv(&argv[i][j]);
				if (value) {
					strcpy(&buffer[k], value);
					k += strlen(&buffer[k]);
				}
				argv[i][l] = tmp;
				j = l - 1;
			}
			else
				buffer[k++] = argv[i][j];
		}
		buffer[k++] = ' ';
	}
	buffer[k] = '\0';

	puts(buffer);
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

int command_cat(int argc, char **argv, char **envp)
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

void format_file_mode(mode_t mode, char *buffer)
{
	mode_t curbit = 0400;

	strcpy(buffer, "-rwxrwxrwx");

	if (mode & S_IFDIR)
		buffer[0] = 'd';

	for (char i = 1; i <= 10; i++) {
		if (!(mode & curbit))
			buffer[i] = '-';
		curbit >>= 1;
	}
}

int command_ls(int argc, char **argv, char **envp)
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

int command_mkdir(int argc, char **argv, char **envp)
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

#define CP_BUF_SIZE	512

int command_cp(int argc, char **argv, char **envp)
{
	int result;
	int src_fd, dest_fd;
	char buffer[CP_BUF_SIZE];

	if (argc <= 2) {
		puts("You need two file names");
		return -1;
	}


	if (!(src_fd = open(argv[1], O_RDONLY, 0))) {
		printf("Error when opening %s: %d\n", argv[1], src_fd);
		return -1;
	}

	if (!(dest_fd = open(argv[2], O_WRONLY | O_CREAT, 0644))) {
		printf("Error when opening %s: %d\n", argv[2], dest_fd);
		return -1;
	}

	while (1) {
		result = read(src_fd, buffer, CP_BUF_SIZE);
		if (result == 0)
			break;

		if (result < 0) {
			printf("Error while reading: %d\n", result);
			return result;
		}

		result = write(dest_fd, buffer, result);

		if (result < 0) {
			printf("Error while writing: %d\n", result);
			return result;
		}
	}

	close(dest_fd);
	close(src_fd);

	return 0;
}

int command_mv(int argc, char **argv, char **envp)
{
	if (argc <= 2) {
		puts("You need two file names");
		return -1;
	}

	int error = rename(argv[1], argv[2]);
	if (error < 0) {
		printf("Error while renaming %s: %d\n", argv[1], error);
		return error;
	}

	return 0;
}

int command_ln(int argc, char **argv, char **envp)
{
	if (argc <= 2) {
		puts("You need two file names");
		return -1;
	}

	int error = link(argv[1], argv[2]);
	if (error < 0) {
		printf("Error while linking %s to %s: %d\n", argv[1], argv[2], error);
		return error;
	}

	return 0;
}

int command_rm(int argc, char **argv, char **envp)
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

int command_chmod(int argc, char **argv, char **envp)
{
	if (argc <= 2) {
		puts("Usage: chmod <mode> <file>");
		return -1;
	}

	for (int i = 0; argv[1][i]; i++) {
		if (argv[1][i] < '0' || argv[1][i] > '7') {
			puts("File mode must be an octal number");
			return -1;
		}
	}

	mode_t mode = strtol(argv[1], NULL, 8);

	int error = chmod(argv[2], mode);
	if (error < 0) {
		printf("Error while renaming %s: %d\n", argv[1], error);
		return error;
	}

	return 0;
}


/*
int command_time(int argc, char **argv, char **envp)
{
	time_t t;
	struct tm *current_time;

	t = time(NULL);
	t = 1604635268;
	current_time = gmtime(&t);

	printf("%d\n", t);
	printf("%d/%02d/%02d %02d:%02d:%02d\n", current_time->tm_year, current_time->tm_mon + 1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

	return 0;
}
*/

#define PS_BUFFER_SIZE	100

int command_ps(int argc, char **argv, char **envp)
{
	pid_t pid;
	int error;
	int dd, fd;
	struct dirent dir;
	char buffer[PS_BUFFER_SIZE];

	if ((dd = open("/proc", 0, 0)) < 0) {
		printf("Error opening /proc\n");
		return dd;
	}

	printf("PID\tCMD\n");
	while (1) {
		error = readdir(dd, &dir);
		if (error < 0) {
			printf("Error at readdir %d\n", error);
			return error;
		}

		if (error == 0)
			break;

		pid = strtol(dir.name, NULL, 10);
		if (pid > 0) {
			snprintf(buffer, PS_BUFFER_SIZE, "/proc/%d/cmdline", pid);
			if ((fd = open(buffer, 0, 0)) < 0) {
				printf("Error opening %s\n", buffer);
				return fd;
			}

			error = read(fd, buffer, PS_BUFFER_SIZE);
			if (error < 0) {
				printf("Error reading %s (%d)\n", buffer, error);
				return error;
			}

			buffer[error] = '\0';

			close(fd);

			printf("%s\t%s\n", dir.name, buffer);
		}
	}

	close(dd);

	return 0;
}

int command_kill(int argc, char **argv, char **envp)
{
	int error;
	int i = 1;
	pid_t pid;
	int signal = 6;
	const char *endptr;

	if (argc <= 1) {
		printf("Usage: kill -[\\d] <pid>\n");
		return 0;
	}

	if (argv[i][0] == '-') {
		signal = strtol(&argv[i][1], &endptr, 10);
		i += 1;
	}

	pid = strtol(argv[i], &endptr, 10);
	if (pid < 0 || endptr[0] != '\0') {
		printf("Invalid pid number\n");
		return -1;
	}

	error = kill(pid, signal);
	if (error < 0) {
		printf("Error attempting to send signal %d to process %d, return %d\n", signal, pid, error);
		return -1;
	}

	return 0;
}

int command_sync(int argc, char **argv, char **envp)
{
	sync();
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

	//add_command("test", 	command_test);
	add_command("dump", 	command_dump);
	add_command("send", 	command_send);
	add_command("echo", 	command_echo);
	add_command("hex", 	command_hex);
	add_command("cat", 	command_cat);
	add_command("ls", 	command_ls);
	add_command("mkdir", 	command_mkdir);
	add_command("cp", 	command_cp);
	add_command("mv", 	command_mv);
	add_command("ln", 	command_ln);
	add_command("rm", 	command_rm);
	add_command("chmod", 	command_chmod);
	//add_command("time", 	command_time);
	add_command("ps", 	command_ps);
	add_command("kill", 	command_kill);
	add_command("sync", 	command_sync);
	add_command("cd", 	command_chdir);
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

char *resolve_file_location(char *filename, char *buffer)
{
	// TODO replace with the environment variable
	const char *PATH = "/bin:/sbin";

	if (strchr(filename, '/')) {
		if (!access(filename, X_OK))
			return filename;
		return NULL;
	}

	short i;
	char *curpath = PATH;
	while (*curpath != '\0') {
		for (i = 0; *curpath && *curpath != ':'; curpath++, i++)
			buffer[i] = *curpath;
		if (*curpath == ':')
			curpath++;
		buffer[i++] = '/';
		strcpy(&buffer[i], filename);

		if (!access(buffer, X_OK))
			return buffer;
	}
	return NULL;
}

int execute_command(struct pipe_command *command, int argc, char **argv, char **envp)
{
	int pid, status;
	char *fullpath;
	char buffer[100];
	void *main = NULL;

	fullpath = resolve_file_location(argv[0], buffer);
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
		ioctl(STDOUT_FILENO, TIOCSPGRP, &fgpid);
	}
	else {
		if (setpgid(0, 0))
			exit(-1);
		pid_t fgpid = getpgid(0);
		ioctl(STDOUT_FILENO, TIOCSPGRP, &fgpid);

		// TODO set the tty's process group to this one? how does it get set back when the process terminates?

		if (command->stdout_file) {
			if (open_file(command->stdout_file, O_WRONLY | O_CREAT | (command->append ? O_APPEND : O_TRUNC), STDOUT_FILENO))
				exit(-1);
		}

		if (main)
			status = SYSCALL3(SYS_EXECBUILTIN, (int) main, (int) argv, (int) envp);
		else
			status = execve(argv[0], argv, envp);
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
	char *envp[2] = { "PATH=/bin:/sbin", NULL };

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
		else if (!strcmp(argv[0], "exit"))
			return;
		else if (!strcmp(argv[0], "cd")) {
			command_chdir(argc, argv, envp);
			continue;
		}

		execute_command(&commands[0], argc, argv, envp);
	}
}

void handle_test(int signum)
{
	//int a = 0xABAB;
	// TODO this exact code and only this code just happens to trigger an optimization where it replaces the arg and jumps to puts instead of generating a proper function
	puts("Hey\n");
	//printf("Hey %d\n", signum);
	//dump((uint16_t *) 0x1179EA, 48);
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
	act.sa_handler = handle_test;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGALRM, &act, NULL);

	//alarm(3);
	//pause();
	//puts("Hey!\n");

	serial_read_loop();

	return 0;
}


