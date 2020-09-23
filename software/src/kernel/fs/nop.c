
#include <errno.h>
#include <kernel/vfs.h>

#include "nop.h"


int nop_create(struct vnode *vnode, const char *filename, mode_t mode, uid_t uid, struct vnode **result)
{
	return EPERM;
}

int nop_mknod(struct vnode *vnode, const char *name, mode_t mode, device_t dev, uid_t uid, struct vnode **result)
{
	return EPERM;
}

int nop_lookup(struct vnode *vnode, const char *name, struct vnode **result)
{
	return ENOENT;
}

int nop_unlink(struct vnode *parent, struct vnode *vnode)
{
	return EPERM;
}

int nop_rename(struct vnode *vnode, struct vnode *oldparent, const char *oldname, struct vnode *newparent, const char *newname)
{
	return EPERM;
}

int nop_truncate(struct vnode *vnode)
{
	return 0;
}

int nop_update(struct vnode *vnode)
{
	return 0;
}

int nop_release(struct vnode *vnode)
{
	return 0;
}

int nop_open(struct vfile *file, int flags)
{
	return EPERM;
}

int nop_close(struct vfile *file)
{
	return 0;
}

int nop_read(struct vfile *file, char *buf, size_t nbytes)
{
	return 0;
}

int nop_write(struct vfile *file, const char *buf, size_t nbytes)
{
	return 0;
}

int nop_ioctl(struct vfile *file, unsigned int request, void *argp)
{
	return EPERM;
}

offset_t nop_seek(struct vfile *file, offset_t position, int whence)
{
	return 0;
}

int nop_readdir(struct vfile *file, struct dirent *dir)
{
	return 0;
}
