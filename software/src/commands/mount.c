
#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>

#include "prototype.h"

int MAIN(command_mount)(int argc, char **argv, char **envp)
{
	int opt;
	int error;
	const char *fstype = "minixfs";
	const char *usage = "Usage: mount [-t <fstype>] <devfile> <mountpoint>";

	optind = 1;
	while ((opt = getopt(argc, argv, "t:")) != EOF) {
		switch (opt) {
		    case '?':
			puts(usage);
			return -1;
		    case 't':
			fstype = optarg;
			break;
		    default:
			printf(">> %c\n", opt);
			break;
		}
	}

	if (argc - optind != 2) {
		puts(usage);
		return -1;
	}

	error = mount(argv[optind], argv[optind + 1], fstype, MOUNT_RW, NULL);
	if (error < 0) {
		printf("Error while mounting %d\n", error);
		return -1;
	}

	return 0;
}


