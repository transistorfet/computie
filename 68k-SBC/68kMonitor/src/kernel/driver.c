
#include <errno.h>
#include <sys/types.h>
#include <kernel/driver.h>

#define MAX_DRIVERS	2

static struct driver *drv_table[MAX_DRIVERS];

int register_driver(devmajor_t major, struct driver *driver)
{
	if (major >= MAX_DRIVERS)
		return -1;

	if (drv_table[major])
		return -1;

	drv_table[major] = driver;
}

int dev_open(device_t dev, int access)
{
	devmajor_t major = dev >> 8;
	devminor_t minor = (devminor_t) dev;

	if (major >= MAX_DRIVERS)
		return ENXIO;
	return drv_table[major]->open(minor, access);
}

int dev_close(device_t dev)
{
	devmajor_t major = dev >> 8;
	devminor_t minor = (devminor_t) dev;

	if (major >= MAX_DRIVERS)
		return ENXIO;
	return drv_table[major]->close(minor);
}

int dev_read(device_t dev, char *buffer, size_t size)
{
	devmajor_t major = dev >> 8;
	devminor_t minor = (devminor_t) dev;

	if (major >= MAX_DRIVERS)
		return ENXIO;
	return drv_table[major]->read(minor, buffer, size);
}

int dev_write(device_t dev, char *buffer, size_t size)
{
	devmajor_t major = dev >> 8;
	devminor_t minor = (devminor_t) dev;

	if (major >= MAX_DRIVERS)
		return ENXIO;
	return drv_table[major]->write(minor, buffer, size);
}

int dev_ioctl(device_t dev, unsigned int request, void *argp)
{
	devmajor_t major = dev >> 8;
	devminor_t minor = (devminor_t) dev;

	if (major >= MAX_DRIVERS)
		return ENXIO;
	return drv_table[major]->ioctl(minor, request, argp);
}

