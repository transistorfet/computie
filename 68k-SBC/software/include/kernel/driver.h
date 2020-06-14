
#ifndef KERNEL_DRIVER_H
#define KERNEL_DRIVER_H

#include <stddef.h>
#include <sys/types.h>

#define DEVMAJOR_TTY	0

typedef unsigned char devmajor_t;
typedef unsigned char devminor_t;

struct driver {
	int (*open)(devminor_t minor, int access);
	int (*close)(devminor_t minor);
	int (*read)(devminor_t minor, char *buffer, size_t size);
	int (*write)(devminor_t minor, const char *buffer, size_t size);
	int (*ioctl)(devminor_t minor, unsigned int request, void *argp);
};


int register_driver(devmajor_t major, struct driver *driver);

int dev_open(device_t dev, int access);
int dev_close(device_t dev);
int dev_read(device_t dev, char *buffer, size_t size);
int dev_write(device_t dev, const char *buffer, size_t size);
int dev_ioctl(device_t dev, unsigned int request, void *argp);

#endif
