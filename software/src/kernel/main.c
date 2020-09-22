
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

#include <kernel/kmalloc.h>
#include <kernel/syscall.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/vfs.h>

#include "proc/process.h"

#include "api.h"
#include "interrupts.h"


extern void init_mallocfs();
extern void init_minix();
extern void tty_68681_preinit();

extern struct driver tty_68681_driver;
extern struct mount_ops mallocfs_mount_ops;
extern struct mount_ops minix_mount_ops;

struct driver *drivers[] = {
	&tty_68681_driver,
	NULL	// Null Termination of Driver List
};


int main()
{
	DISABLE_INTS();

	tty_68681_preinit();

	init_kernel_heap((void *) 0x110000, 0xD0000);

	init_interrupts();
	init_syscall();
	init_proc();

	init_vfs();
	init_mallocfs();
	init_minix();

	// TODO this would be moved elsewhere
	struct mount *mp;
	//vfs_mount(NULL, "/", 0, &mallocfs_mount_ops, SU_UID, &mp);
	vfs_mount(NULL, "/", 1, &minix_mount_ops, SU_UID, &mp);

	// Initialize drivers
	for (char i = 0; drivers[i]; i++) {
		drivers[i]->init();
	}

/*
	{
		int error;
		struct vfile *file;
		error = vfs_open(NULL, "/mnt", O_CREAT, S_IFDIR | 0755, SU_UID, &file);
		if (error)
			printk("Error: %d\n", error);
		else
			vfs_close(file);

		struct mount *mp2;
		error = vfs_mount(NULL, "/mnt", 1, &minix_mount_ops, SU_UID, &mp2);
		if (error)
			printk("Mount error: %d\n", error);
	}
*/

	begin_multitasking(create_init_task());
}

