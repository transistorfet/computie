
#include <stdint.h>

#include <sys/stat.h>
#include <kernel/inode.h>
#include <kernel/driver.h>

#include "../interrupts.h"


int tty_68681_open(devminor_t minor, int access);
int tty_68681_close(devminor_t minor);
int tty_68681_read(devminor_t minor, char *buffer, size_t size);
int tty_68681_write(devminor_t minor, const char *buffer, size_t size);
int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp);

struct driver tty_68681_driver = {
	tty_68681_open,
	tty_68681_close,
	tty_68681_read,
	tty_68681_write,
	tty_68681_ioctl,
};

// MC68681 Register Addresses
#define MR1A_MR2A_ADDR	((volatile uint8_t *) 0x700001)
#define SRA_RD_ADDR	((volatile uint8_t *) 0x700003)
#define CSRA_WR_ADDR	((volatile uint8_t *) 0x700003)
#define CRA_WR_ADDR	((volatile uint8_t *) 0x700005)
#define TBA_WR_ADDR	((volatile uint8_t *) 0x700007)
#define RBA_RD_ADDR	((volatile uint8_t *) 0x700007)
#define ACR_WR_ADDR	((volatile uint8_t *) 0x700009)

#define CTUR_WR_ADDR	((volatile uint8_t *) 0x70000D)
#define CTLR_WR_ADDR	((volatile uint8_t *) 0x70000F)
#define START_RD_ADDR	((volatile uint8_t *) 0x70001D)
#define STOP_RD_ADDR	((volatile uint8_t *) 0x70001F)

#define IPCR_RD_ADDR	((volatile uint8_t *) 0x700009)
#define OPCR_WR_ADDR	((volatile uint8_t *) 0x70001B)
#define INPUT_RD_ADDR	((volatile uint8_t *) 0x70001B)
#define OUT_SET_ADDR	((volatile uint8_t *) 0x70001D)
#define OUT_RESET_ADDR	((volatile uint8_t *) 0x70001F)

#define ISR_RD_ADDR	((volatile uint8_t *) 0x70000B)
#define IMR_WR_ADDR	((volatile uint8_t *) 0x70000B)
#define IVR_WR_ADDR	((volatile uint8_t *) 0x700019)


// MC68681 Command Numbers
#define CMD_ENABLE_TX_RX		0x05


// MC68681 Default Configuration Values
#define MR1A_MODE_A_REG_1_CONFIG	0b10010011	// RTS Enabled, 8 bits, No Parity
#define MR2A_MODE_A_REG_2_CONFIG	0b00010111	// Normal mode, CTS Enabled, 1 stop bit
#define CSRA_CLK_SELECT_REG_A_CONFIG	0b10111011	// 9600 bps @ 3.6864MHz (19200 @ 7.3728 MHz)
#define ACR_AUX_CONTROL_REG_CONFIG	0b11110000	// Set2, External Clock / 16, IRQs disabled

// Interrupt Status/Mask Bits
#define ISR_INPUT_CHANGE		0x80
#define ISR_CH_B_BREAK_CHANGE		0x40
#define ISR_CH_B_RX_READY_FULL		0x20
#define ISR_CH_B_TX_READY		0x10
#define ISR_TIMER_CHANGE		0x08
#define ISR_CH_A_BREAK_CHANGE		0x04
#define ISR_CH_A_RX_READY_FULL		0x02
#define ISR_CH_A_TX_READY		0x01

#define TTY_INT_VECTOR			IV_USER_VECTORS


__attribute__((interrupt)) void handle_serial_irq();

struct inode *tty_inode = NULL;

// TODO remove after debugging
char tick = 0;

int init_tty()
{
	// Configure channel A serial port
	*MR1A_MR2A_ADDR = MR1A_MODE_A_REG_1_CONFIG;
	*MR1A_MR2A_ADDR = MR2A_MODE_A_REG_2_CONFIG;
	*CSRA_WR_ADDR = CSRA_CLK_SELECT_REG_A_CONFIG;
	*ACR_WR_ADDR = ACR_AUX_CONTROL_REG_CONFIG;

	// Enable the channel A serial port
	*CRA_WR_ADDR = CMD_ENABLE_TX_RX;


	// Configure timer
	*CTUR_WR_ADDR = 0xFF;
	*CTLR_WR_ADDR = 0xFF;

	// Enable interrupts
	set_interrupt(TTY_INT_VECTOR, handle_serial_irq);
	*IVR_WR_ADDR = TTY_INT_VECTOR;
	*IMR_WR_ADDR = ISR_TIMER_CHANGE;


	register_driver(DEVMAJOR_TTY, &tty_68681_driver);
	// TODO this is a hack because we don't have another way of creating the inode yet
	tty_inode = new_inode((DEVMAJOR_TTY << 8), S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO);


	// Flash LEDs briefly at boot
	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;
	*OUT_RESET_ADDR = 0xF0;
}

int getchar(void)
{
	char in;

	while (1) {
		if (*SRA_RD_ADDR & 0x01)
			return *RBA_RD_ADDR;

		/*
		// Debugging - Set all LEDs on if an input is active
		in = (*INPUT_RD_ADDR & 0x3f);
		if (in & 0x18) {
			*OUT_SET_ADDR = 0xF0;
		} else {
		*OUT_RESET_ADDR = 0xF0;
		}
		*/

		/*
		// Debugging - Set LEDs on timer
		if (*ISR_RD_ADDR & ISR_TIMER_CHANGE) {
			tick = !tick;
			*OUT_SET_ADDR = tick;
			*OUT_RESET_ADDR = !tick;
		}
		*/
	}
}

int putchar(int ch)
{
	while (!(*SRA_RD_ADDR & 0x04)) { }
	*TBA_WR_ADDR = (char) ch;
	return ch;
}


int tty_68681_open(devminor_t minor, int access)
{

}

int tty_68681_close(devminor_t minor)
{

}

int tty_68681_read(devminor_t minor, char *buffer, size_t size)
{
	for (; size > 0; size--, buffer++)
		*buffer = getchar();
}

int tty_68681_write(devminor_t minor, const char *buffer, size_t size)
{
	for (; size > 0; size--, buffer++)
		putchar(*buffer);
}

int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp)
{

}


__attribute__((interrupt)) void handle_serial_irq()
{
/*
	register char isr = *ISR_RD_ADDR;

	if (isr & ISR_TIMER_CHANGE) {
		register char data = *STOP_RD_ADDR;
		//tty_68681_write(0, "Time\n", 5);
		tick = !tick;
		*((volatile unsigned char *) 0x201D) = tick;
	}


	// TODO this is required to reset a pin change interrupt
	uint8_t status = *IPCR_RD_ADDR;
*/
	if (*ISR_RD_ADDR & ISR_TIMER_CHANGE) {
		if (tick) {
			tick = 0;
			*OUT_SET_ADDR = 0x80;
		} else {
			tick = 1;
			*OUT_RESET_ADDR = 0x80;
		}

		//run_task();

		register char reset = *STOP_RD_ADDR;
	}
}


