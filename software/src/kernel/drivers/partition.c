
#include <asm/macros.h>

#include "partition.h"


struct partition_entry {
	uint8_t status;
	uint8_t chs_start[3];
	uint8_t fstype;
	uint8_t chs_end[3];
	uint32_t lba_start;
	uint32_t lba_sectors;
};

int read_partition_table(char *buffer, struct partition *devices)
{
	struct partition_entry *table;

	table = (struct partition_entry *) &buffer[0x1BE];

	for (short i = 0; i < 4; i++) {
		devices[i].base = from_le32(table[i].lba_start);
		devices[i].size = from_le32(table[i].lba_sectors);
		devices[i].fstype = table[i].fstype;
		devices[i].flags = table[i].status;
	}

	return 0;
}


