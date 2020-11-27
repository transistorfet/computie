
#ifndef KERNEL_DRIVER_H
#define KERNEL_DRIVER_H

#include <stddef.h>
#include <sys/types.h>

#define DEVMAJOR_TTY	1
#define DEVMAJOR_MEM	2
#define DEVMAJOR_ATA	3

#define DEVNUM(major, minor)	((major) << 8 | (minor))

typedef unsigned char devmajor_t;
typedef unsigned char devminor_t;

struct driver {
	int (*init)();
	int (*open)(devminor_t minor, int access);
	int (*close)(devminor_t minor);
	int (*read)(devminor_t minor, char *buffer, offset_t offset, size_t size);
	int (*write)(devminor_t minor, const char *buffer, offset_t offset, size_t size);
	int (*ioctl)(devminor_t minor, unsigned int request, void *argp);
	offset_t (*seek)(devminor_t minor, offset_t position, int whence, offset_t offset);
};


int register_driver(devmajor_t major, struct driver *driver);

int dev_open(device_t dev, int access);
int dev_close(device_t dev);
int dev_read(device_t dev, char *buffer, offset_t offset, size_t size);
int dev_write(device_t dev, const char *buffer, offset_t offset, size_t size);
int dev_ioctl(device_t dev, unsigned int request, void *argp);
offset_t dev_seek(device_t dev, offset_t position, int whence, offset_t offset);


#endif
