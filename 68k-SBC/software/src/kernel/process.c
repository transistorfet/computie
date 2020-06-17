
#include <kernel/filedesc.h>

#include "process.h"

#define PROCESS_MAX	6
static int next_pid = 1;
static struct process table[PROCESS_MAX];

void init_proc()
{
	for (char i = 0; i < PROCESS_MAX; i++) {
		table[i].pid = 0;
	}
}

struct process *new_proc(struct inode *inode)
{
	for (char i = 0; i < PROCESS_MAX; i++) {
		if (!table[i].pid) {
			table[i].pid = next_pid++;
			init_fd_table(table[i].fd_table);

			// TODO use the inode to load an executable into memory?


			return &table[i];
		}
	}
	return NULL;
}

void free_proc(int pid)
{
	for (char i = 0; i < PROCESS_MAX; i++) {
		if (table[i].pid == pid) {
			table[i].pid = 0;
			// TODO free all the things
			break;
		}
	}
}
