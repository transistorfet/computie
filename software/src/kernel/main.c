
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


extern void tty_68681_preinit();

extern struct driver tty_68681_driver;
extern struct driver mem_driver;

struct driver *drivers[] = {
	&tty_68681_driver,
	&mem_driver,
	NULL	// Null Termination of Drivers List
};

extern struct mount_ops mallocfs_mount_ops;
extern struct mount_ops minix_mount_ops;

struct mount_ops *filesystems[] = {
	&mallocfs_mount_ops,
	&minix_mount_ops,
	NULL	// Null Termination of Filesystems List
};

int main()
{
	DISABLE_INTS();

	tty_68681_preinit();

	init_kernel_heap((void *) 0x110000, 0xD0000);

	init_interrupts();
	init_syscall();
	init_proc();

	// TODO This is a temporary hack until driver init and vfs access is separated
	register_driver(DEVMAJOR_MEM, &mem_driver);

	init_vfs();

	// Initialize drivers
	for (char i = 0; filesystems[i]; i++) {
		filesystems[i]->init();
	}

	// TODO this would be moved elsewhere
	struct mount *mp;
	//vfs_mount(NULL, "/", 0, &mallocfs_mount_ops, SU_UID, &mp);
	vfs_mount(NULL, "/", DEVNUM(DEVMAJOR_MEM, 0), &minix_mount_ops, SU_UID, &mp);

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

