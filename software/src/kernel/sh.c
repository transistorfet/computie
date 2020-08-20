

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/syscall.h>


void delay(short count) {
	while (--count > 0) { }
}

int readline(char *buffer, short max)
{
	uint32_t *sp;

	for (short i = 0; i < max; i++) {
		//buffer[i] = getchar();

		int ret = read(0, &buffer[i], 1);
		putchar(buffer[i]);
		if (ret != 1) {
			printf("Input error: %d\n", ret);
		}
		else if (buffer[i] == 0x08) {
			i -= 2;
		}
		else if (buffer[i] == '\n') {
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
		write(fd, (char *) &data, 2);
	}

	close(fd);

	puts("Load complete");
}

#define DUMP_BUF_SIZE	0x20

void dump_file(const char *name)
{
	int fd;
	int result;
	char buffer[DUMP_BUF_SIZE];

	if ((fd = open(name, O_RDONLY)) < 0) {
		printf("Error opening %s: %d\n", name, fd);
		return;
	}


	while (1) {
		result = read(fd, buffer, DUMP_BUF_SIZE);
		if (result == 0)
			break;

		if (result < 0) {
			printf("Error while reading: %d\n", result);
			return;
		}

		dump((uint16_t *) &buffer, result >> 1);
	}

	close(fd);
}



void sh_fork()
{
	//int pid = SYSCALL1(SYS_FORK, 0);
	int pid = fork();

	if (pid) {
		int status;

		printf("The child's pid is %d\n", pid);
		wait(&status);
		printf("The child exited with %d\n", status);
	}
	else {
		puts("I AM THE CHILD!");
		exit(1337);
	}
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

void ls(char *path)
{
	int fd;
	int error;
	struct vdir dir;
	struct vfile *file;
	struct stat statbuf;
	char filename[100];
	char filemode[10];

	if ((fd = open(path, 0)) < 0) {
		printf("Error opening %s: %d\n", path, fd);
		return;
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
			return;
		}

		if (error == 0)
			break;

		strcpy(&filename[start], dir.name);
		error = stat(filename, &statbuf);
		if (error < 0) {
			printf("Error at stat %s (%d)\n", filename, error);
			return;
		}

		format_file_mode(statbuf.st_mode, filemode);
		printf("%s %6d %s\n", filemode, statbuf.st_size, dir.name);
	}

	close(fd);
}


void command_unlink(char *path)
{
	int error = unlink(path);
	if (error < 0) {
		printf("Error while unlinking %s: %d\n", path, error);
	}
}



void command_exec(char *path)
{
	int pid, status;
	char *argv[2] = { "an arg", NULL };
	char *envp[2] = { NULL };

 	pid = fork();
	if (pid) {
		printf("The child's pid is %d\n", pid);
		// TODO this is the correct way, but I've commented it out because it caught a bug where fork was called before assigning the values to argv, and by that time
		//	the parent has exited this function and is blocking on a read.  Would switching which proc gets run first after a fork have an affect on this?
		wait(&status);
		printf("The child exited with %d\n", status);
	}
	else {
		status = exec(path, argv, envp);
		// The exec() system call will only return if an error occurs
		printf("Failed to execute %s: %d\n", path, status);
		exit(-1);
	}
}

void sh_pipe()
{
	int error;
	int pipes[2];
	char buffer[50];

	error = pipe(pipes);
	if (error) {
		printf("Pipe failed with error %d\n", error);
		return;
	}

	error = write(pipes[PIPE_WRITE_FD], "Hey there, this is a pipe|\n", 27);
	if (error < 0) {
		printf("Failed to write to pipe: %d\n", error);
		return;
	}
	printf("Wrote %d bytes\n", error);

	error = read(pipes[PIPE_READ_FD], buffer, 50);
	if (error < 0) {
		printf("Failed to read to pipe: %d\n", error);
		return;
	}
	printf("Read %d bytes\n", error);
	buffer[error] = '\0';

	printf("> %s\n", buffer);

	close(pipes[PIPE_READ_FD]);
	close(pipes[PIPE_WRITE_FD]);

	return;
}

void sh_forkpipe()
{
	int pid;
	int error;
	int pipes[2];
	char buffer[50];

	error = pipe(pipes);
	if (error) {
		printf("Pipe failed with error %d\n", error);
		return;
	}

 	pid = fork();
	if (pid) {
		close(pipes[PIPE_WRITE_FD]);

		error = read(pipes[PIPE_READ_FD], buffer, 50);
		if (error < 0) {
			printf("Failed to read to pipe: %d\n", error);
			return;
		}
		printf("Read %d bytes\n", error);
		buffer[error] = '\0';

		printf("> %s\n", buffer);

		close(pipes[PIPE_READ_FD]);

	} else {
		close(pipes[PIPE_READ_FD]);

		error = write(pipes[PIPE_WRITE_FD], "Hey there, this is a pipe|\n", 27);
		if (error < 0) {
			printf("Failed to write to pipe: %d\n", error);
			return;
		}
		printf("Wrote %d bytes\n", error);

		close(pipes[PIPE_WRITE_FD]);
		exit(0);
	}

	return;
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
		putsn("% ");
		readline(buffer, BUF_SIZE);
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
		else if (!strcmp(args[0], "more")) {
			if (argc <= 1)
				puts("You need file name");
			else {
				dump_file(args[1]);
			}
		}
		else if (!strcmp(args[0], "fork")) {
			sh_fork();
		}
		else if (!strcmp(args[0], "ls")) {
			ls(argc > 1 ? args[1] : "/");
		}
		else if (!strcmp(args[0], "rm")) {
			if (argc <= 1)
				puts("You need file name");
			else {
				command_unlink(args[1]);
			}
		}
		else if (!strcmp(args[0], "pipetest")) {
			sh_pipe();
		}
		else if (!strcmp(args[0], "forkpipe")) {
			sh_forkpipe();
		}
		else if (!strcmp(args[0], "exit")) {
			return;
		}
		else if (*args[0]) {
			puts("Command not found");
		}
	}
}



void file_test()
{
	int error;
	struct vfile *file;

	/*
	error = vfs_create(mallocfs_root, "dir", S_IFDIR | 0755, &vn);
	if (error)
		printk("Error: %d\n", error);
	else
		printk("Created dir at %x\n", vn->block);

	error = mallocfs_create(vn, "test", 0644, &vn);
	if (error)
		printk("Error: %d\n", error);
	else
		printk("Created at %x\n", vn->block);
	*/

	if ((error = vfs_open("test", O_CREAT, &file))) {
		printk("Error at open %d\n", error);
		return;
	}

	if ((error = vfs_write(file, "This is a file test\n", 20)) <= 0) {
		printk("Error when writing %d\n", error);
		return;
	}

	vfs_seek(file, 0, 0);

	char buffer[256];

	error = vfs_read(file, buffer, 256);
	if (error < 0) {
		printk("Error when reading\n");
		return;
	}
	printk("Read: %d\n", error);
	buffer[error] = '\0';

	puts(buffer);

	vfs_close(file);


	extern const char hello_task[800];
	if ((error = vfs_open("/hello", O_CREAT, &file))) {
		printk("Error at open new file %d\n", error);
		return;
	}

	if ((error = vfs_write(file, hello_task, 256)) <= 0) {
		printk("Error when writing %d\n", error);
		return;
	}

	vfs_close(file);

	puts("done");

	if ((error = vfs_open("/size", O_CREAT, &file))) {
		printk("Error at open new file %d\n", error);
		return;
	}

	char data[2];
	for (short i = 0; i < 1000; i++) {
		for (short j = 0; j < 10; j++) {
			data[0] = '0' + j;
			//printk("%d\n", data[0]);
			if ((error = vfs_write(file, data, 1)) <= 0) {
				printk("Error when writing %d\n", error);
				return;
			}
		}
	}

	vfs_close(file);

}

void dir_test()
{
	int error;
	struct vfile *file;
	struct vdir dir;

	if ((error = vfs_open("/", 0, &file))) {
		printk("Error at open %d\n", error);
		return;
	}

	while (1) {
		error = vfs_readdir(file, &dir);
		if (error < 0) {
			printk("Error at readdir %d\n", error);
			return;
		}
		else if (error == 0)
			break;
		else {
			printk("File: %d:%s\n", error, dir.name);
		}
	}


}





int sh_task()
{
	file_test();
	dir_test();


	// Allocate memory for loading a processess
	program_mem = malloc(0x1000);

	puts("\n\nThe Pseudo Shell!\n");

	serial_read_loop();

	puts("Exiting to monitor");
	return 0;
}


