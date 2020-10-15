
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/kmalloc.h>
#include <kernel/syscall.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/scheduler.h>

#include "proc/tasks.h"
#include "proc/process.h"

#include "api.h"
#include "time.h"
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


void create_dir_or_panic(const char *path)
{
	struct vfile *file;
	struct vnode *vnode;

	if (!vfs_lookup(NULL, path, SU_UID, VLOOKUP_NORMAL, &vnode))
		vfs_release_vnode(vnode);
	else {
		if (vfs_open(NULL, path, O_CREAT, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO, SU_UID, &file))
			panic("Unable to create %s\n", path);
		vfs_close(file);
	}
}

void create_special_or_panic(const char *path, device_t rdev)
{
	struct vnode *vnode;

	if (vfs_lookup(NULL, path, SU_UID, VLOOKUP_NORMAL, &vnode))
		if (vfs_mknod(NULL, path, S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, rdev, SU_UID, &vnode))
			panic("Unable to create special file %s\n", path);
	vfs_release_vnode(vnode);
}

int main()
{
	DISABLE_INTS();

	tty_68681_preinit();

	init_kernel_heap((void *) 0x110000, 0xD0000);

	init_time();
	init_interrupts();
	init_syscall();
	init_proc();
	init_scheduler();

	// Initialize drivers
	for (char i = 0; drivers[i]; i++) {
		drivers[i]->init();
	}

	init_vfs();

	// Initialize drivers
	for (char i = 0; filesystems[i]; i++) {
		filesystems[i]->init();
	}

	// TODO this would be moved elsewhere
	//vfs_mount(NULL, "/", 0, &mallocfs_mount_ops, SU_UID);
	vfs_mount(NULL, "/", DEVNUM(DEVMAJOR_MEM, 0), &minix_mount_ops, SU_UID);


	create_dir_or_panic("/bin");
	create_dir_or_panic("/dev");

	create_special_or_panic("/dev/tty0", DEVNUM(DEVMAJOR_TTY, 0));
	create_special_or_panic("/dev/tty1", DEVNUM(DEVMAJOR_TTY, 1));
	create_special_or_panic("/dev/mem0", DEVNUM(DEVMAJOR_MEM, 0));

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

	begin_multitasking();
}

