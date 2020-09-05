
#ifndef _SRC_KERNEL_ACCESS_H
#define _SRC_KERNEL_ACCESS_H

#define S_READ		04
#define S_WRITE		02
#define S_EXECUTE	01

int verify_mode_access(uid_t current_uid, mode_t require_mode, uid_t file_uid, gid_t file_gid, mode_t file_mode);

#endif

