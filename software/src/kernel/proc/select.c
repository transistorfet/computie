
#include <sys/types.h>
#include <sys/select.h>

#include <kernel/vfs.h>
#include <kernel/scheduler.h>

#include "process.h"

int select_check_open_fds(struct process *proc, int max, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
	int fd = 0;
	fd_mask_t mask;

	for (short word = 0; word < _FDS_MAX / __FDS_BITS; word++) {
		mask = 0;
		if (readfds)
			mask |= readfds->fds_bits[word];
		if (writefds)
			mask |= writefds->fds_bits[word];
		if (exceptfds)
			mask |= exceptfds->fds_bits[word];
		for (fd_mask_t bit = 0x01; bit; bit <<= 1, fd++) {
			if (fd >= max)
				return 0;
			if ((mask & bit) && !proc->fd_table[fd])
				return -1;
		}
	}
	return 0;
}

int select_poll_fds(struct process *proc, int max, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, char mark_ready)
{
	int fd = 0;
	int revents;
	int ready = 0;

	for (short word = 0; word < _FDS_MAX / __FDS_BITS; word++) {
		for (fd_mask_t bit = 0x01; bit; bit <<= 1, fd++) {
			if (fd >= max)
				break;

			// TODO should it just poll for any of the 3, or trim it down to just the ones we're looking for
			revents = vfs_poll(proc->fd_table[fd], VFS_POLL_READ | VFS_POLL_WRITE | VFS_POLL_ERROR);

			if (readfds && (readfds->fds_bits[word] & bit)) {
				if (revents & VFS_POLL_READ)
					ready += 1;
				else if (mark_ready)
					readfds->fds_bits[word] &= ~bit;
			}
			if (writefds && (writefds->fds_bits[word] & bit)) {
				if (revents & VFS_POLL_WRITE)
					ready += 1;
				else if (mark_ready)
					writefds->fds_bits[word] &= ~bit;
			}
			if (exceptfds && (exceptfds->fds_bits[word] & bit)) {
				if (revents & VFS_POLL_ERROR)
					ready += 1;
				else if (mark_ready)
					exceptfds->fds_bits[word] &= ~bit;
			}

			if (!mark_ready && ready)
				return 1;
		}

	}
	return ready;
}

int select_wait_check(struct process *proc, int events, struct vnode *vnode, device_t rdev)
{
	if (select_poll_fds(proc, (int) proc->blocked_call.arg1, (fd_set *) proc->blocked_call.arg2, (fd_set *) proc->blocked_call.arg3, (fd_set *) proc->blocked_call.arg4, 0))
		return 1;
	return 0;
}

static int select_timeout(struct timer *timer, struct process *proc)
{
	resume_proc(proc);
}

int enter_select(struct process *proc, int max, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	// When entering select for the first time, we do some initial checks
	if (!(proc->bits & PB_SELECT)) {
		if (select_check_open_fds(proc, max, readfds, writefds, exceptfds))
			return -1;

		// If no files are ready, then suspend the process for the timeout
		if (!select_poll_fds(proc, max, readfds, writefds, exceptfds, 0)) {
			if (timeout) {
				// If a timeout of zero is specified, then return immediately instead of suspending
				if (timeout->tv_sec == 0 && timeout->tv_usec == 0)
					return 0;
				current_proc->timer.argp = current_proc;
				current_proc->timer.callback = (timer_callback_t) select_timeout;
				add_timer(&current_proc->timer, timeout->tv_sec, timeout->tv_usec);
			}

			//suspend_proc(proc, PB_SELECT);
			//proc->blocked_call = *current_syscall;
			//proc->wait_check = select_wait_check;
			suspend_syscall(proc, PB_SELECT, select_wait_check, current_syscall);
			return 0;
		}
	}

	proc->bits &= ~PB_SELECT;
	proc->wait_check = NULL;

	return select_poll_fds(proc, max, readfds, writefds, exceptfds, 1);
}


