
#include <errno.h>
#include <sys/types.h>
#include <kernel/driver.h>

#include "nop.h"
#include "device.h"

struct vfile_ops device_vfile_ops = {
	device_ops_open,
	device_ops_close,
	device_ops_read,
	device_ops_write,
	device_ops_ioctl,
	device_ops_poll,
	device_ops_seek,
	nop_readdir,
};

int device_ops_open(struct vfile *file, int flags)
{
	return dev_open(file->vnode->rdev, flags);
}

int device_ops_close(struct vfile *file)
{
	return dev_close(file->vnode->rdev);
}

int device_ops_read(struct vfile *file, char *buffer, size_t size)
{
	int nbytes;

	nbytes = dev_read(file->vnode->rdev, buffer, file->position, size);
	if (nbytes < 0)
		return nbytes;
	file->position += nbytes;
	return nbytes;
}

int device_ops_write(struct vfile *file, const char *buffer, size_t size)
{
	int nbytes;

	nbytes = dev_write(file->vnode->rdev, buffer, file->position, size);
	if (nbytes < 0)
		return nbytes;
	file->position += nbytes;
	return nbytes;
}

int device_ops_ioctl(struct vfile *file, unsigned int request, void *argp, uid_t uid)
{
	return dev_ioctl(file->vnode->rdev, request, argp, uid);
}

int device_ops_poll(struct vfile *file, int events)
{
	return dev_poll(file->vnode->rdev, events);
}

offset_t device_ops_seek(struct vfile *file, offset_t position, int whence)
{
	int pos;

	pos = dev_seek(file->vnode->rdev, position, whence, file->position);
	if (pos < 0)
		return pos;
	file->position = pos;
	return pos;
}

