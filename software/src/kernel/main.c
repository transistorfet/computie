
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <kernel/vfs.h>
#include <kernel/time.h>
#include <kernel/kmalloc.h>
#include <kernel/syscall.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <kernel/scheduler.h>

#include "proc/tasks.h"
#include "proc/process.h"

#include "net/if.h"
#include "net/protocol.h"

#include "api.h"
#include "interrupts.h"


extern void tty_68681_preinit();

extern struct driver tty_68681_driver;
extern struct driver tty_driver;
extern struct driver mem_driver;
extern struct driver ata_driver;

struct driver *drivers[] = {
	&tty_68681_driver,
	&tty_driver,
	&mem_driver,
	&ata_driver,
	NULL	// Null Termination of Drivers List
};

extern struct mount_ops mallocfs_mount_ops;
extern struct mount_ops minix_mount_ops;
extern struct mount_ops procfs_mount_ops;

struct mount_ops *filesystems[] = {
	//&mallocfs_mount_ops,
	&minix_mount_ops,
	&procfs_mount_ops,
	NULL	// Null Termination of Filesystems List
};

extern struct if_ops slip_if_ops;
extern struct protocol_ops ipv4_protocol_ops;
extern struct protocol_ops udp_protocol_ops;

char boot_args[32] = "mem0";
device_t root_dev = DEVNUM(DEVMAJOR_MEM, 0);


void create_dir_or_panic(const char *path)
{
	struct vfile *file;
	struct vnode *vnode;

	if (!vfs_lookup(NULL, path, SU_UID, VLOOKUP_NORMAL, &vnode))
		vfs_release_vnode(vnode);
	else {
		if (vfs_open(NULL, path, O_CREAT, S_IFDIR | 0755, SU_UID, &file))
			panic("Unable to create %s\n", path);
		vfs_close(file);
	}
}

void create_special_or_panic(const char *path, device_t rdev)
{
	struct vnode *vnode;

	if (vfs_lookup(NULL, path, SU_UID, VLOOKUP_NORMAL, &vnode))
		if (vfs_mknod(NULL, path, S_IFCHR | 0755, rdev, SU_UID, &vnode))
			panic("Unable to create special file %s\n", path);
	vfs_release_vnode(vnode);
}

void parse_boot_args()
{
	// TODO this is overly simplistic because there aren't many options yet
	if (!*boot_args || !strcmp(boot_args, "mem0"))
		root_dev = DEVNUM(DEVMAJOR_MEM, 0);
	else if (!strcmp(boot_args, "ata0"))
		root_dev = DEVNUM(DEVMAJOR_ATA, 0);
}

int main()
{
	DISABLE_INTS();

	tty_68681_preinit();

	printk_safe("\nBooting with \"%s\"...\n\n", boot_args);
	parse_boot_args();

	init_kernel_heap((void *) 0x110000, 0xD0000);

	init_time();
	init_interrupts();
	init_syscall();
	init_proc();
	init_scheduler();

	// Initialize drivers before VFS
	for (char i = 0; drivers[i]; i++) {
		drivers[i]->init();
	}

	init_vfs();

	// Initialize specific filesystems
	for (char i = 0; filesystems[i]; i++) {
		filesystems[i]->init();
	}

	init_net_if();
	init_net_protocol();
	slip_if_ops.init();
	ipv4_protocol_ops.init();
	udp_protocol_ops.init();
	// TODO this is a temporary hack.  The ifup should be done later
	net_if_up("slip0");

	// TODO this would be moved elsewhere
	//vfs_mount(NULL, "/", 0, &mallocfs_mount_ops, SU_UID);
	printk_safe("minixfs: mounting (%x) at %s\n", root_dev, "/");
	vfs_mount(NULL, "/", root_dev, &minix_mount_ops, 0, SU_UID);

	create_dir_or_panic("/bin");
	create_dir_or_panic("/dev");
	create_dir_or_panic("/proc");
	create_dir_or_panic("/media");

	create_special_or_panic("/dev/tty0", DEVNUM(DEVMAJOR_TTY, 0));
	create_special_or_panic("/dev/tty1", DEVNUM(DEVMAJOR_TTY, 1));
	create_special_or_panic("/dev/mem0", DEVNUM(DEVMAJOR_MEM, 0));
	create_special_or_panic("/dev/ata0", DEVNUM(DEVMAJOR_ATA, 0));

	// TODO device number here is an issue because 0 is used to indicated a mount slot is not used, which when mounting after this causes a /proc error
	printk_safe("procfs: mounting at /proc\n");
	vfs_mount(NULL, "/proc", 1, &procfs_mount_ops, VFS_MBF_READ_ONLY, SU_UID);

	//vfs_mount(NULL, "/media", DEVNUM(DEVMAJOR_ATA, 0), &minix_mount_ops, SU_UID);

	begin_multitasking();
}

