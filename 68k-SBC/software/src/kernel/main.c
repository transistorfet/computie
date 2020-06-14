
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/filedesc.h>
#include <kernel/syscall.h>

#include "interrupts.h"


extern void sh_task();


extern void init_tty();
extern void init_syscall();


extern struct inode *tty_inode;
fd_table_t proc_fd_table;

int main()
{
	//load_data_segment();
	init_heap((void *) 0x110000, 0x1000);
	init_interrupts();
	init_syscall();

	init_inode();
	init_fd_table(proc_fd_table);
	init_tty();



	int fd = new_fd(proc_fd_table, tty_inode);


	struct file *f = get_fd(proc_fd_table, fd);
	//if (!f)
	//	return 0;
	dev_write(f->inode->device, "\nTEST\n", 6);




	//puts("\n\nWelcome to the \x1b[32mOS!\n");
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

	sh_task();

	return 0;
}

