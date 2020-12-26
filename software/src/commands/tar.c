
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TAR_BLOCK_SIZE		512

struct tar_header {
	char filename[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char filesize[12];
	char mtime[12];
	char checksum[8];
	char type;
	char linkname[100];
	char ustar_sig[6];
	char ustar_ver[2];
	char owner[32];
	char group[32];
	char device_major[8];
	char device_minor[8];
	char prefix[155];
};

int tar_read(int fd);
int tar_verify_checksum(struct tar_header *header, unsigned char *buffer);
int tar_read_regular_file(int tarfd, const char *filename, mode_t mode, int filesize);

int main(int argc, char **argv)
{
	int fd;
	int error;

	if (argc < 2) {
		printf("Usage: tar <filename>\n");
		return 1;
	}

	if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
		printf("Error opening %s\n", argv[1]);
		return fd;
	}

	error = tar_read(fd);

	close(fd);
	return error;
}

int tar_read(int fd)
{
	int mode;
	int error;
	int filesize;
	char filename[256];
	struct tar_header *header;
	unsigned char buffer[TAR_BLOCK_SIZE];

	while (1) {
		error = read(fd, buffer, TAR_BLOCK_SIZE);
		if (error < 0)
			return error;
		if (error != TAR_BLOCK_SIZE)
			break;
		header = (struct tar_header *) buffer;

		error = tar_verify_checksum(header, buffer);
		if (error < 0)
			return error;
		if (error > 0)
			continue;

		filesize = strtol(header->filesize, NULL, 8);
		mode = strtol(header->mode, NULL, 8);
		strcpy(filename, header->prefix);
		strcat(filename, header->filename);

		// TODO this is a temporary hack because mkdir doesn't support trailing slashes
		int len = strlen(filename);
		if (filename[len - 1] == '/')
			filename[len - 1] = '\0';

		printf("%s (%x) %c\n", filename, mode, header->type);

		switch (header->type) {
			case '0':
				error = tar_read_regular_file(fd, filename, mode, filesize);
				if (error < 0)
					return error;
				break;
			case '5':
				error = mkdir(filename, mode);
				if (error < 0)
					return error;
				break;
			default:
				printf("Unexpected file type %c\n", header->type);
				return -1;
		}
	}

	return 0;
}

int tar_verify_checksum(struct tar_header *header, unsigned char *buffer)
{
	int zeros;
	int checksum;
	int checksum_rec;

	// If the checksum is already NUL then don't modify so the zero check still passes
	if (header->checksum[0] != '\0') {
		checksum_rec = strtol(header->checksum, NULL, 8);
		for (short i = 0; i < 8; i++)
			header->checksum[i] = 0x20;
	}

	zeros = 1;
	checksum = 0;
	for (short i = 0; i < TAR_BLOCK_SIZE; i++) {
		if (buffer[i])
			zeros = 0;
		checksum += buffer[i];
	}

	if (zeros)
		return 1;
	if (checksum != checksum_rec) {
		printf("Error: checksum failed: %d but expected %d\n", checksum, checksum_rec);
		return -1;
	}
	if (strncmp(header->ustar_sig, "ustar", 5)) {
		printf("Error: expected ustar signature, but none found\n");
		return -1;
	}
	return 0;
}

int tar_read_regular_file(int tarfd, const char *filename, mode_t mode, int filesize)
{
	int error;
	int filefd;
	unsigned char buffer[TAR_BLOCK_SIZE];

	if ((filefd = open(filename, O_WRONLY | O_CREAT, mode)) < 0) {
		printf("Error opening %s: %d\n", filename, filefd);
		return filefd;
	}

	for (; filesize > 0; filesize -= TAR_BLOCK_SIZE) {
		error = read(tarfd, buffer, TAR_BLOCK_SIZE);
		if (error < 0) {
			printf("Error while reading file: %d\n", error);
			return error;
		}

		error = write(filefd, buffer, filesize < TAR_BLOCK_SIZE ? filesize : TAR_BLOCK_SIZE);
		if (error < 0) {
			printf("Error while writing file: %d\n", error);
			return error;
		}
	}

	close(filefd);
	return 0;
}

