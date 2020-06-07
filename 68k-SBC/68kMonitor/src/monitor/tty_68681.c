
#include <stdint.h>
#include "tty.h"

// MC68681 Register Addresses
#define MR1A_MR2A_ADDR	((uint8_t *) 0x700001)
#define SRA_RD_ADDR	((uint8_t *) 0x700003)
#define CSRA_WR_ADDR	((uint8_t *) 0x700003)
#define CRA_WR_ADDR	((uint8_t *) 0x700005)
#define TBA_WR_ADDR	((uint8_t *) 0x700007)
#define RBA_RD_ADDR	((uint8_t *) 0x700007)
#define ACR_WR_ADDR	((uint8_t *) 0x700009)

#define CTUR_WR_ADDR	((uint8_t *) 0x70000D)
#define CTLR_WR_ADDR	((uint8_t *) 0x70000F)
#define START_RD_ADDR	((uint8_t *) 0x70001D)
#define STOP_RD_ADDR	((uint8_t *) 0x70001F)

#define IPCR_RD_ADDR	((uint8_t *) 0x700009)
#define OPCR_WR_ADDR	((uint8_t *) 0x70001B)
#define INPUT_RD_ADDR	((uint8_t *) 0x70001B)
#define OUT_SET_ADDR	((uint8_t *) 0x70001D)
#define OUT_RESET_ADDR	((uint8_t *) 0x70001F)

#define ISR_RD_ADDR	((uint8_t *) 0x70000B)
#define IMR_WR_ADDR	((uint8_t *) 0x70000B)
#define IVR_WR_ADDR	((uint8_t *) 0x700019)


// MC68681 Command Numbers
#define CMD_ENABLE_TX_RX		0x05


// MC68681 Default Configuration Values
#define MR1A_MODE_A_REG_1_CONFIG	0b10010011	// RTS Enabled, 8 bits, No Parity
#define MR2A_MODE_A_REG_2_CONFIG	0b00010111	// Normal mode, CTS Enabled, 1 stop bit
#define CSRA_CLK_SELECT_REG_A_CONFIG	0b10111011	// 9600 bps @ 3.6864MHz (19200 @ 7.3728 MHz)
#define ACR_AUX_CONTROL_REG_CONFIG	0b11110000	// Set2, External Clock / 16, IRQs disabled

#define TTY_READ_BUFFER		32

static char tty_read_in = 0;
static char tty_read_out = 0;
static char tty_read_buffer[TTY_READ_BUFFER];

int init_tty()
{
	*MR1A_MR2A_ADDR = MR1A_MODE_A_REG_1_CONFIG;
	*MR1A_MR2A_ADDR = MR2A_MODE_A_REG_2_CONFIG;
	*CSRA_WR_ADDR = CSRA_CLK_SELECT_REG_A_CONFIG;
	*ACR_WR_ADDR = ACR_AUX_CONTROL_REG_CONFIG;

	*CRA_WR_ADDR = CMD_ENABLE_TX_RX;

	// Enable rx interrupt
	//*IVR_WR_ADDR = 0x07;
	//*IMR_WR_ADDR = 0x02;

	// Enable pin change interrupts (test)
	//*IVR_WR_ADDR = 0x0f;
	//*IMR_WR_ADDR = 0x80;

	// Turn ON Test LED
	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;
	*OUT_RESET_ADDR = 0xF0;
}

int getchar(void)
{
	char in;

	while (1) {
		//if (tty_read_out < tty_read_in)
		//	return tty_read_buffer[tty_read_out++];
		in = *SRA_RD_ADDR;
		if (in & 0x01)
			return *RBA_RD_ADDR;

		// Debugging - Set LEDs to the upper status bits of channel A
		if (in & 0xF0) {
			*OUT_SET_ADDR = in;
			*OUT_RESET_ADDR = (~in & 0xF0);
		}

		/*
		// Debugging - Set all LEDs on if an input is active
		in = (*INPUT_RD_ADDR & 0x3f);
		if (in & 0x18) {
			*OUT_SET_ADDR = 0xF0;
		} else {
			*OUT_RESET_ADDR = 0xF0;
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

__attribute__((interrupt)) void handle_serial_irq()
{
	while (*SRA_RD_ADDR & 0x01) {
		tty_read_buffer[tty_read_in++] = *RBA_RD_ADDR;
	}

	char in = *SRA_RD_ADDR;
	if (in & 0xF0) {
		*OUT_SET_ADDR = in;
		*OUT_RESET_ADDR = (~in & 0xF0);
	}
}

