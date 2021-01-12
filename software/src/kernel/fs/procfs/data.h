
#ifndef _SRC_KERNEL_FS_PROCFS_DATA_H
#define _SRC_KERNEL_FS_PROCFS_DATA_H

struct process;

int get_data_cmdline(struct process *proc, char *buffer, int max);
int get_data_stat(struct process *proc, char *buffer, int max);
int get_data_statm(struct process *proc, char *buffer, int max);

#endif
