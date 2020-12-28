
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>
#include <asm/macros.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/driver.h>

#include "../interrupts.h"


// Driver Definition
int ata_init();
int ata_open(devminor_t minor, int access);
int ata_close(devminor_t minor);
int ata_read(devminor_t minor, char *buffer, offset_t offset, size_t size);
int ata_write(devminor_t minor, const char *buffer, offset_t offset, size_t size);
int ata_ioctl(devminor_t minor, unsigned int request, void *argp);
offset_t ata_seek(devminor_t minor, offset_t position, int whence, offset_t offset);


struct driver ata_driver = {
	ata_init,
	ata_open,
	ata_close,
	ata_read,
	ata_write,
	ata_ioctl,
	ata_seek,
};


// Driver-specific data
struct ata_geometry {
	int base;
	size_t size;
};

#define ATA_DEV_MAX		4

static struct ata_geometry devices[ATA_DEV_MAX];


#define ATA_REG_DEV_CONTROL	((volatile uint8_t *) 0x60001D)
#define ATA_REG_DEV_ADDRESS	((volatile uint8_t *) 0x60001F)
#define ATA_REG_DATA		((volatile uint16_t *) 0x600020)
#define ATA_REG_DATA_BYTE	((volatile uint8_t *) 0x600021)
#define ATA_REG_FEATURE		((volatile uint8_t *) 0x600023)
#define ATA_REG_ERROR		((volatile uint8_t *) 0x600023)
#define ATA_REG_SECTOR_COUNT	((volatile uint8_t *) 0x600025)
#define ATA_REG_SECTOR_NUM	((volatile uint8_t *) 0x600027)
#define ATA_REG_CYL_LOW		((volatile uint8_t *) 0x600029)
#define ATA_REG_CYL_HIGH	((volatile uint8_t *) 0x60002B)
#define ATA_REG_DRIVE_HEAD	((volatile uint8_t *) 0x60002D)
#define ATA_REG_STATUS		((volatile uint8_t *) 0x60002F)
#define ATA_REG_COMMAND		((volatile uint8_t *) 0x60002F)


#define ATA_CMD_READ_SECTORS	0x20
#define ATA_CMD_WRITE_SECTORS	0x30
#define ATA_CMD_IDENTIFY	0xEC
#define ATA_CMD_SET_FEATURE	0xEF

#define ATA_ST_BUSY		0x80
#define ATA_ST_DATA_READY	0x08
#define ATA_ST_ERROR		0x01


#define ATA_DELAY(x)		{ for (int delay = 0; delay < (x); delay++) { asm volatile(""); } }
#define ATA_WAIT()		{ while (*ATA_REG_STATUS & ATA_ST_BUSY) { } }
#define ATA_WAIT_FOR_DATA()	{ while (!(*ATA_REG_STATUS) & ATA_ST_DATA_READY) { } }

/*
static inline void ATA_DELAY(short delay)
{
	for (; delay > 0; delay--) { asm volatile(""); }
}

static inline void ATA_WAIT()
{
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
}
*/

int ata_detect()
{
	uint8_t status;

	status = *ATA_REG_STATUS;
	// If the busy bit is already set, or the two bits that are always 0, then perhaps nothing is connected
	if (status & (ATA_ST_BUSY | 0x06))
		return 0;
	ATA_DELAY(10);

	// Reset the IDE bus
	(*ATA_REG_COMMAND) = ATA_CMD_IDENTIFY;

	for (int i = 0; i < 1000; i++) {
		ATA_DELAY(10);

		status = *ATA_REG_STATUS;
		// If it becomes unbusy within the timeout then a drive is connected
		if (!(status & ATA_ST_BUSY)) {
			if (status & ATA_ST_DATA_READY) {
				ATA_DELAY(100);
				return 1;
			}
			else
				return 0;
		}
	}
	return 0;
}

/*
int ata_reset()
{
	(*ATA_REG_DEV_CONTROL) = 0x04;
	ATA_DELAY(16);
	(*ATA_REG_DEV_CONTROL) = 0x00;
	ATA_DELAY(16);
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
	return 0;
}
*/

int ata_read_sector(int sector, char *buffer)
{
	short saved_status;

	LOCK(saved_status);

	ATA_DELAY(10);

	// Set 8-bit mode
	(*ATA_REG_FEATURE) = 0x01;
	(*ATA_REG_COMMAND) = ATA_CMD_SET_FEATURE;
	ATA_DELAY(10);
	ATA_WAIT();

	// Read a sector
	(*ATA_REG_DRIVE_HEAD) = 0xE0;
	//(*ATA_REG_DRIVE_HEAD) = 0xE0 | (uint8_t) ((sector >> 24) & 0x0F);
	(*ATA_REG_CYL_HIGH) = (uint8_t) (sector >> 16);
	(*ATA_REG_CYL_LOW) = (uint8_t) (sector >> 8);
	(*ATA_REG_SECTOR_NUM) = (uint8_t) sector;
	(*ATA_REG_SECTOR_COUNT) = 1;
	(*ATA_REG_COMMAND) = ATA_CMD_READ_SECTORS;
	ATA_DELAY(10);
	ATA_WAIT();

	char status = (*ATA_REG_STATUS);
	if (status & 0x01) {
		printk_safe("Error while reading ata: %x\n", (*ATA_REG_ERROR));
		UNLOCK(saved_status);
		return 0;
	}

	ATA_DELAY(10);
	ATA_WAIT();
	ATA_WAIT_FOR_DATA();

	for (int i = 0; i < 512; i++) {
		//((uint16_t *) buffer)[i] = (*ATA_REG_DATA);
		//asm volatile("rol.w	#8, %0\n" : "+g" (((uint16_t *) buffer)[i]));
		buffer[i] = (*ATA_REG_DATA_BYTE);

		ATA_DELAY(10);
		ATA_WAIT();
		ATA_DELAY(10);
	}

	/*
	printk_safe("Mem %x:\n", sector);
	for (int i = 0; i < 512; i++) {
		printk_safe("%x ", 0xff & buffer[i]);
		if ((i & 0x1F) == 0x1F)
			printk_safe("\n");
	}
	*/

	UNLOCK(saved_status);
	return 512;
}

int ata_write_sector(int sector, const char *buffer)
{
	short saved_status;

	LOCK(saved_status);

	// Set 8-bit mode
	(*ATA_REG_FEATURE) = 0x01;
	(*ATA_REG_COMMAND) = ATA_CMD_SET_FEATURE;
	ATA_WAIT();

	// Read a sector
	(*ATA_REG_DRIVE_HEAD) = 0xE0;
	//(*ATA_REG_DRIVE_HEAD) = 0xE0 | (uint8_t) ((sector >> 24) & 0x0F);
	(*ATA_REG_CYL_HIGH) = (uint8_t) (sector >> 16);
	(*ATA_REG_CYL_LOW) = (uint8_t) (sector >> 8);
	(*ATA_REG_SECTOR_NUM) = (uint8_t) sector;
	(*ATA_REG_SECTOR_COUNT) = 1;
	(*ATA_REG_COMMAND) = ATA_CMD_WRITE_SECTORS;
	ATA_WAIT();

	char status = (*ATA_REG_STATUS);
	//printk_safe("IDE: %x\n", status);
	if (status & 0x01) {
		printk_safe("Error while writing ata: %x\n", (*ATA_REG_ERROR));
		UNLOCK(saved_status);
		return 0;
	}

	ATA_DELAY(100);
	ATA_WAIT();
	ATA_WAIT_FOR_DATA();

	for (int i = 0; i < 512; i++) {
		ATA_WAIT();
		//while (((*ATA_REG_STATUS) & ATA_ST_BUSY) || !((*ATA_REG_STATUS) & ATA_ST_DATA_READY)) { }

		//((uint16_t *) buffer)[i] = (*ATA_REG_DATA);
		//asm volatile("rol.w	#8, %0\n" : "+g" (((uint16_t *) buffer)[i]));
		(*ATA_REG_DATA_BYTE) = buffer[i];
	}

	ATA_WAIT();

	if (*ATA_REG_STATUS & ATA_ST_ERROR) {
		printk_safe("Error writing sector %d: %x\n", sector, *ATA_REG_ERROR);
	}

	UNLOCK(saved_status);
	return 512;
}


// TODO this should be moved into its own driver-independent place
struct partition_entry {
	uint8_t status;
	uint8_t chs_start[3];
	uint8_t fstype;
	uint8_t chs_end[3];
	uint32_t lba_start;
	uint32_t lba_sectors;
};

int load_partition_table()
{
	char buffer[512];
	struct partition_entry *table;

	ata_read_sector(0, buffer);
	table = (struct partition_entry *) &buffer[0x1BE];

	for (short i = 0; i < 4; i++) {
		devices[i].base = from_le32(table[i].lba_start);
		devices[i].size = from_le32(table[i].lba_sectors);
		if (devices[i].size)
			printk_safe("ata%d: found partition with %d sectors\n", i, devices[i].size);
	}

	return 0;
}


int ata_init()
{

	for (short i = 0; i < ATA_DEV_MAX; i++)
		devices[i].base = 0;
	register_driver(DEVMAJOR_ATA, &ata_driver);

	// TODO this doesn't work very well
	if (!ata_detect()) {
		printk_safe("ATA device not detected\n");
		return 0;
	}

	load_partition_table();

	return 0;
}

int ata_open(devminor_t minor, int access)
{
	if (minor >= ATA_DEV_MAX || devices[minor].base == 0)
		return ENXIO;
	return 0;
}

int ata_close(devminor_t minor)
{
	if (minor >= ATA_DEV_MAX)
		return ENXIO;
	return 0;
}

int ata_read(devminor_t minor, char *buffer, offset_t offset, size_t size)
{
	if (minor >= ATA_DEV_MAX || devices[minor].base == 0)
		return ENXIO;
	struct ata_geometry *geo = &devices[minor];

	if (offset > (geo->size << 9))
		return -1;
	if (offset + size > (geo->size << 9))
		size = (geo->size << 9) - offset;

	offset >>= 9;
	for (int count = size >> 9; count > 0; count--, offset++, buffer = &buffer[512])
		ata_read_sector(geo->base + offset, buffer);
	return size;
}

int ata_write(devminor_t minor, const char *buffer, offset_t offset, size_t size)
{
	if (minor >= ATA_DEV_MAX || devices[minor].base == 0)
		return ENXIO;
	struct ata_geometry *geo = &devices[minor];

	if (offset > (geo->size << 9))
		return -1;
	if (offset + size > (geo->size << 9))
		size = (geo->size << 9) - offset;

	offset >>= 9;
	for (int count = size >> 9; count > 0; count--, offset++, buffer = &buffer[512])
		ata_write_sector(geo->base + offset, buffer);
	return size;
}

int ata_ioctl(devminor_t minor, unsigned int request, void *argp)
{
	if (minor >= ATA_DEV_MAX || devices[minor].base == 0)
		return ENXIO;
	return -1;
}

offset_t ata_seek(devminor_t minor, offset_t position, int whence, offset_t offset)
{
	if (minor >= ATA_DEV_MAX || devices[minor].base == 0)
		return ENXIO;
	struct ata_geometry *geo = &devices[minor];

	switch (whence) {
	    case SEEK_SET:
		break;
	    case SEEK_CUR:
		position = offset + position;
		break;
	    case SEEK_END:
		position = geo->size + position;
		break;
	    default:
		return EINVAL;
	}

	if (position > geo->size)
		position = geo->size;
	return position;

}
