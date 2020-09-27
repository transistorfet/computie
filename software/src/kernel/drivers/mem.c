
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/driver.h>


// Driver Definition
int mem_init();
int mem_open(devminor_t minor, int access);
int mem_close(devminor_t minor);
int mem_read(devminor_t minor, char *buffer, offset_t offset, size_t size);
int mem_write(devminor_t minor, const char *buffer, offset_t offset, size_t size);
int mem_ioctl(devminor_t minor, unsigned int request, void *argp);


struct driver mem_driver = {
	mem_init,
	mem_open,
	mem_close,
	mem_read,
	mem_write,
	mem_ioctl,
};

struct mem_geometry {
	char *base;
	size_t size;
};

static int num_devices = 1;
static struct mem_geometry devices[] = {
	{ (char *) 0x1E0000, 0x20000 },
};


int mem_init()
{
	register_driver(DEVMAJOR_MEM, &mem_driver);
	return 0;
}

int mem_open(devminor_t minor, int access)
{
	if (minor >= num_devices)
		return ENXIO;
	return 0;
}

int mem_close(devminor_t minor)
{
	if (minor >= num_devices)
		return ENXIO;
	return 0;
}

int mem_read(devminor_t minor, char *buffer, offset_t offset, size_t size)
{
	if (minor >= num_devices)
		return ENXIO;
	struct mem_geometry *geo = &devices[minor];

	if (offset > geo->size)
		return -1;
	if (offset + size > geo->size)
		size = geo->size - offset;
	memcpy_s(buffer, &geo->base[offset], size);
	return size;
}

int mem_write(devminor_t minor, const char *buffer, offset_t offset, size_t size)
{
	if (minor >= num_devices)
		return ENXIO;
	struct mem_geometry *geo = &devices[minor];

	if (offset > geo->size)
		return -1;
	if (offset + size > geo->size)
		size = geo->size - offset;
	memcpy_s(&geo->base[offset], buffer, size);
	return size;
}

int mem_ioctl(devminor_t minor, unsigned int request, void *argp)
{
	if (minor >= num_devices)
		return ENXIO;
	return -1;
}


