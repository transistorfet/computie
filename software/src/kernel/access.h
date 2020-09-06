
#ifndef _SRC_KERNEL_ACCESS_H
#define _SRC_KERNEL_ACCESS_H

int verify_mode_access(uid_t current_uid, mode_t require_mode, uid_t file_uid, gid_t file_gid, mode_t file_mode);

#endif

