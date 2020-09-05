
#include <sys/types.h>

#include "access.h"


int verify_mode_access(uid_t current_uid, mode_t require_mode, uid_t file_uid, gid_t file_gid, mode_t file_mode)
{
	if (current_uid == SU_UID || current_uid == file_uid) {
		return require_mode & ((file_mode >> 6) & 0x07);
	}
	else {
		return require_mode & (file_mode & 0x07);
	}
}

