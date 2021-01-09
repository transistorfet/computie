
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioc_tty.h>
#include <kernel/driver.h>
#include <kernel/syscall.h>
#include <kernel/signal.h>
#include <kernel/scheduler.h>

#include "../interrupts.h"


#define TTY_DEVICE_NUM		2

#define MAX_CANNON		256

struct tty_device {
	device_t rdev;
	pid_t pgid;

	struct termios tio;

	int error;
	char ready;
	char escape;
	short opens;
	short buf_read;
	short buf_write;
	char buffer[MAX_CANNON];
};

struct termios default_tio = {
	.c_iflag = (BRKINT | ICRNL | IXON | IXANY),
	.c_oflag = (OPOST | ONLCR),
	.c_cflag = (CREAD | CS8 | HUPCL),
	.c_lflag = (ISIG | IEXTEN | ICANON | ECHO | ECHOE),
	.c_cc = {
		'\x04',		// VEOF
		0xff,		// VEOL
		'\x08',		// VERASE
		'\x03',		// VINTR
		'\x15',		// VKILL
		1,		// VMIN
		'\x1C',		// VQUIT
		0,		// VTIME
		'\x1A',		// VSUSP
		'\x13',		// VSTART
		'\x11',		// VSTOP
	}
};


// Driver Definition
int tty_init();
int tty_open(devminor_t minor, int access);
int tty_close(devminor_t minor);
int tty_read(devminor_t minor, char *buffer, offset_t offset, size_t size);
int tty_write(devminor_t minor, const char *buffer, offset_t offset, size_t size);
int tty_ioctl(devminor_t minor, unsigned int request, void *argp);

struct driver tty_driver = {
	tty_init,
	tty_open,
	tty_close,
	tty_read,
	tty_write,
	tty_ioctl,
};


static struct tty_device devices[TTY_DEVICE_NUM];

#define READ_BUF	256

static inline void tty_read_input(device_t minor)
{
	int read;
	char buffer[READ_BUF];
	struct tty_device *tty = &devices[minor];

	// If the buffer is full, then consider the line to be terminated and resume the waiting proc
	if (tty->buf_write >= MAX_CANNON) {
		tty->ready = 1;
		resume_blocked_procs(SYS_READ, NULL, DEVNUM(DEVMAJOR_TTY, minor));
		return;
	}

	read = dev_read(tty->rdev, buffer, 0, READ_BUF - tty->buf_write);
	if (read < 0) {
		tty->error = read;
		return;
	}

	for (int i = 0; i < read; i++) {
		if ((tty->tio.c_lflag & ISIG)) {
			if (buffer[i] == tty->tio.c_cc[VINTR]) {
				//tty->ready = 1;
				//resume_blocked_procs(SYS_READ, NULL, DEVNUM(DEVMAJOR_TTY, minor));
				send_signal_process_group(tty->pgid, SIGINT);
				return;
			}
			//else if (buffer[i] == tty->tio.c_cc[VSUSP]) {
			//	send_signal_process_group(tty->pgid, SIGSTOP);
			//	return;
			//}
		}


		if (buffer[i] == '\x1B') {
			tty->escape = 1;
		}
		else if (tty->escape == 1) {
			tty->escape = (buffer[i] == '[') ? 2 : 0;
		}
		else if (tty->escape == 2) {
			// TODO intepret the next character (or buffer somewhere if not a letter
			tty->escape = 0;
		}
		else if (buffer[i] == tty->tio.c_cc[VERASE]) {
			if (tty->buf_write >= 1) {
				tty->buf_write--;
				if ((tty->tio.c_lflag & ECHOE)) {
					if (tty->buffer[tty->buf_write] == '\t')
						dev_write(tty->rdev, "\x08\x08\x08\x08\x08\x08\x08\x08", 0, 3);
					else
						dev_write(tty->rdev, "\x08 \x08", 0, 3);
				}
			}
			break;
		}
		else if (buffer[i] == '\r') {
			// Ignore
		}
		else {
			tty->buffer[tty->buf_write++] = buffer[i];
			if ((tty->tio.c_lflag & ECHO))
				dev_write(tty->rdev, &buffer[i], 0, 1);
			if (buffer[i] == '\n') {
				tty->ready = 1;
				resume_blocked_procs(SYS_READ, NULL, DEVNUM(DEVMAJOR_TTY, minor));
			}
		}
	}
}

static void tty_process_input(void *_unused)
{
	for (short minor = 0; minor < TTY_DEVICE_NUM; minor++) {
		if (devices[minor].opens && (devices[minor].tio.c_lflag & ICANON) && !devices[minor].ready)
			tty_read_input(minor);
	}
}

int tty_init()
{
	for (short i = 0; i < TTY_DEVICE_NUM; i++) {
		devices[i].rdev = DEVNUM(DEVMAJOR_TTY68681, i);
		devices[i].pgid = 0;
		devices[i].error = 0;
		devices[i].opens = 0;
		devices[i].ready = 0;
		devices[i].buf_read = 0;
		devices[i].buf_write = 0;

		memcpy(&devices[i].tio, &default_tio, sizeof(struct termios));
	}

	register_driver(DEVMAJOR_TTY, &tty_driver);
	register_bh(SH_TTY, tty_process_input, NULL);
}

int tty_open(devminor_t minor, int mode)
{
	if (minor >= TTY_DEVICE_NUM)
		return ENODEV;
	// TODO you need to notify the serial driver to request the tty bottom half run after any input
	if (devices[minor].opens++ == 0)
		return dev_open(devices[minor].rdev, mode | O_NONBLOCK);
	return 0;
}

int tty_close(devminor_t minor)
{
	if (minor >= TTY_DEVICE_NUM)
		return ENODEV;
	if (devices[minor].opens == 0)
		return EBADF;
	if (--devices[minor].opens == 0)
		return dev_close(devices[minor].rdev);
	return 0;
}

int tty_read(devminor_t minor, char *buffer, offset_t offset, size_t size)
{
	if (minor >= TTY_DEVICE_NUM)
		return ENODEV;

	// If an error occurred when buffering the data, then return it and clear the error
	if (devices[minor].error) {
		int error = devices[minor].error;
		devices[minor].error = 0;
		return error;
	}

	if (!(devices[minor].tio.c_lflag & ICANON)) {
		if (devices[minor].buf_read < devices[minor].buf_write) {
			size_t max = devices[minor].buf_write - devices[minor].buf_read;
			if (size < max)
				max = size;
			memcpy(buffer, &devices[minor].buffer[devices[minor].buf_read], max);
			devices[minor].buf_read += max;
			return max;
		}

		int read = dev_read(devices[minor].rdev, buffer, offset, size);
		if (read == 0) {
			suspend_current_proc();
			return 0;
		}
		return read;
	}
	else {
		// If an entire line is not available yet, then suspend the process
		if (!devices[minor].ready) {
			suspend_current_proc();
			return 0;
		}

		// Copy the lesser of the remaining buffered bytes or the requested size
		size_t max = devices[minor].buf_write - devices[minor].buf_read;
		if (size < max)
			max = size;
		for (short i = 0; i < max; i++)
			buffer[i] = devices[minor].buffer[devices[minor].buf_read++];

		// If an entire line has been read, then reset the buffer and attempt to read more input from the raw device
		if (devices[minor].buf_read >= devices[minor].buf_write) {
			devices[minor].ready = 0;
			devices[minor].buf_read = 0;
			devices[minor].buf_write = 0;
			request_bh_run(SH_TTY);
		}
		return max;
	}
}

int tty_write(devminor_t minor, const char *buffer, offset_t offset, size_t size)
{
	int written;

	if (minor >= TTY_DEVICE_NUM)
		return ENODEV;

	written = dev_write(devices[minor].rdev, buffer, offset, size);
	if (!written) {
		suspend_current_proc();
		return 0;
	}
	return written;
}

int tty_ioctl(devminor_t minor, unsigned int request, void *argp)
{
	short saved_status;

	if (minor >= TTY_DEVICE_NUM)
		return ENODEV;

	switch (request) {
		case TCGETS: {
			LOCK(saved_status);
			memcpy((struct termios *) argp, &devices[minor].tio, sizeof(struct termios));
			UNLOCK(saved_status);
			return 0;
		}
		case TCSETS: {
			LOCK(saved_status);
			memcpy(&devices[minor].tio, (struct termios *) argp, sizeof(struct termios));
			UNLOCK(saved_status);
			return 0;
		}

		case TIOCGPGRP: {
			*((int *) argp) = devices[minor].pgid;
			return 0;
		}
		case TIOCSPGRP: {
			devices[minor].pgid = *((int *) argp);
			return 0;
		}
		default:
			return dev_ioctl(devices[minor].rdev, request, argp);
	}
	return -1;
}



