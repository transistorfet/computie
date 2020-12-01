
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
	char *base;
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

/*
void ata_test()
{
	// Reset the IDE bus
	(*ATA_REG_DEV_CONTROL) = 0x04;
	for (int i = 0; i < 16; i++) { asm volatile(""); }
	(*ATA_REG_DEV_CONTROL) = 0x00;
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }


	printk_safe("IDE: %x\n", (*ATA_REG_STATUS));
	//printk_safe("ADDR: %x\n", (*ATA_REG_DEV_ADDRESS));

	//(*ATA_REG_COMMAND) = ATA_CMD_IDENTIFY;
	//while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	//(*ATA_REG_COMMAND) = 0x0008;
	//while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	// Set 8-bit mode
	//(*ATA_REG_FEATURE) = 0x01;
	//(*ATA_REG_COMMAND) = ATA_CMD_SET_FEATURE;
	//while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	char buffer[2048];

//for (int j = 0; j < 4; j++) {
	// 0x802 is the superblock for the minix partition
	(*ATA_REG_DRIVE_HEAD) = 0xE0;
	(*ATA_REG_CYL_HIGH) = 0x00;
	(*ATA_REG_CYL_LOW) = 0x00;
	(*ATA_REG_SECTOR_NUM) = 0;
	(*ATA_REG_SECTOR_COUNT) = 4;
	(*ATA_REG_COMMAND) = ATA_CMD_READ_SECTORS;
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }


	char status = (*ATA_REG_STATUS);
	printk_safe("IDE: %x\n", status);
	if (status & 0x01)
		return;

	delay(100);
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	int read = 0;
	while ((*ATA_REG_STATUS) & ATA_ST_DATA_READY) {
		for (int i = 0; i < 256; i++) {
			while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
			//while (((*ATA_REG_STATUS) & ATA_ST_BUSY) || !((*ATA_REG_STATUS) & ATA_ST_DATA_READY)) { }

			((uint16_t *) buffer)[read + i] = (*ATA_REG_DATA);
			asm volatile("rol.w	#8, %0\n" : "+g" (((uint16_t *) buffer)[read + i]));
			//buffer[read + i] = (*ATA_REG_DATA_BYTE);

			//printk_safe("%x ", ((uint16_t *) buffer)[i]);
			//if ((i & 0x0F) == 0xF)
			//	printk_safe("\n");
		}
		read += 256;
	}

	//delay(1000);

	printk_safe("Mem:\n");
	for (int i = 0; i < 2048; i++) {
		printk_safe("%x ", 0xff & buffer[i]);
		if ((i & 0x1F) == 0x1F)
			printk_safe("\n");
	}
//}
}
*/

#define ATA_DELAY(x)	{ for (int delay = 0; delay < (x); delay++) { asm volatile(""); } }


int ata_read_sector(int sector, char *buffer)
{
	short saved_status;

	LOCK(saved_status);

	ATA_DELAY(100);

	// Set 8-bit mode
	(*ATA_REG_FEATURE) = 0x01;
	(*ATA_REG_COMMAND) = ATA_CMD_SET_FEATURE;
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	// Read a sector
	(*ATA_REG_DRIVE_HEAD) = 0xE0;
	//(*ATA_REG_DRIVE_HEAD) = 0xE0 | (uint8_t) ((sector >> 24) & 0x0F);
	(*ATA_REG_CYL_HIGH) = (uint8_t) (sector >> 16);
	(*ATA_REG_CYL_LOW) = (uint8_t) (sector >> 8);
	(*ATA_REG_SECTOR_NUM) = (uint8_t) sector;
	(*ATA_REG_SECTOR_COUNT) = 1;
	(*ATA_REG_COMMAND) = ATA_CMD_READ_SECTORS;
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	char status = (*ATA_REG_STATUS);
	//printk_safe("IDE: %x\n", status);
	if (status & 0x01) {
		printk_safe("Error while reading ata: %x\n", (*ATA_REG_ERROR));
		UNLOCK(saved_status);
		return 0;
	}

	ATA_DELAY(100);
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
	while (!(*ATA_REG_STATUS) & ATA_ST_DATA_READY) { }

	for (int i = 0; i < 512; i++) {
		ATA_DELAY(10);
		while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
		//while (((*ATA_REG_STATUS) & ATA_ST_BUSY) || !((*ATA_REG_STATUS) & ATA_ST_DATA_READY)) { }

		//((uint16_t *) buffer)[i] = (*ATA_REG_DATA);
		//asm volatile("rol.w	#8, %0\n" : "+g" (((uint16_t *) buffer)[i]));
		buffer[i] = (*ATA_REG_DATA_BYTE);
	}

	printk_safe("Mem %x:\n", sector);
	for (int i = 0; i < 512; i++) {
		printk_safe("%x ", 0xff & buffer[i]);
		if ((i & 0x1F) == 0x1F)
			printk_safe("\n");
	}

	UNLOCK(saved_status);
	return 512;
}

int ata_write_sector(int sector, char *buffer)
{
	short saved_status;

	LOCK(saved_status);

	// Set 8-bit mode
	(*ATA_REG_FEATURE) = 0x01;
	(*ATA_REG_COMMAND) = ATA_CMD_SET_FEATURE;
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	// Read a sector
	(*ATA_REG_DRIVE_HEAD) = 0xE0;
	//(*ATA_REG_DRIVE_HEAD) = 0xE0 | (uint8_t) ((sector >> 24) & 0x0F);
	(*ATA_REG_CYL_HIGH) = (uint8_t) (sector >> 16);
	(*ATA_REG_CYL_LOW) = (uint8_t) (sector >> 8);
	(*ATA_REG_SECTOR_NUM) = (uint8_t) sector;
	(*ATA_REG_SECTOR_COUNT) = 1;
	(*ATA_REG_COMMAND) = ATA_CMD_WRITE_SECTORS;
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }

	char status = (*ATA_REG_STATUS);
	//printk_safe("IDE: %x\n", status);
	if (status & 0x01) {
		printk_safe("Error while writing ata: %x\n", (*ATA_REG_ERROR));
		UNLOCK(saved_status);
		return 0;
	}

	ATA_DELAY(100);
	while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
	while (!(*ATA_REG_STATUS) & ATA_ST_DATA_READY) { }

	for (int i = 0; i < 512; i++) {
		while (*ATA_REG_STATUS & ATA_ST_BUSY) { }
		//while (((*ATA_REG_STATUS) & ATA_ST_BUSY) || !((*ATA_REG_STATUS) & ATA_ST_DATA_READY)) { }

		//((uint16_t *) buffer)[i] = (*ATA_REG_DATA);
		//asm volatile("rol.w	#8, %0\n" : "+g" (((uint16_t *) buffer)[i]));
		(*ATA_REG_DATA_BYTE) = buffer[i];
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


int ata_init()
{
	struct partition_entry *table;

	//ata_test();

	char buffer[512];
	ata_read_sector(0, buffer);
	table = &buffer[0x1BE];

	for (short i = 0; i < 4; i++) {
		devices[i].base = from_le32(table[i].lba_start);
		devices[i].size = from_le32(table[i].lba_sectors);
		printk_safe("Partition %d: %x size %x\n", i, devices[i].base, devices[i].size);
	}


	//devices[0].base = 0x800;
	//devices[0].size = 0xA000;
	register_driver(DEVMAJOR_ATA, &ata_driver);
	return 0;
}

int ata_open(devminor_t minor, int access)
{
	if (minor >= ATA_DEV_MAX)
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
	if (minor >= ATA_DEV_MAX)
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
	if (minor >= ATA_DEV_MAX)
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
	if (minor >= ATA_DEV_MAX)
		return ENXIO;
	return -1;
}

offset_t ata_seek(devminor_t minor, offset_t position, int whence, offset_t offset)
{
	if (minor >= ATA_DEV_MAX)
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
