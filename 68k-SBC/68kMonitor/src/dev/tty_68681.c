
#include "tty.h"

#define MR1A_MODE_A_REG_1_CONFIG	0b10010011	// RTS Enabled, 8 bits, No Parity
#define MR2A_MODE_A_REG_2_CONFIG	0b00010111	// Normal mode, CTS Enabled, 1 stop bit
#define CSRA_CLK_SELECT_REG_A_CONFIG	0b10111011	// 9600 bps @ 3.6864MHz (19200 @ 7.3728 MHz)
#define ACR_AUX_CONTROL_REG_CONFIG	0b11110000	// Set2, External Clock / 16, IRQs disabled

#define CMD_ENABLE_TX_RX		0x05

//#define MR1A_MR2A_ADDR	0x800001
//#define SRA_RD_ADDR	0x800003
//#define CSRA_WR_ADDR	0x800003
//#define CRA_WR_ADDR	0x800005
//#define TBA_WR_ADDR	0x800007
//#define RBA_RD_ADDR	0x800007
//#define ACR_ADDR	0x800009

#define MR1A_MR2A_ADDR	0x4001
#define SRA_RD_ADDR	0x4003
#define CSRA_WR_ADDR	0x4003
#define CRA_WR_ADDR	0x4005
#define TBA_WR_ADDR	0x4007
#define RBA_RD_ADDR	0x4007
#define ACR_WR_ADDR	0x4009
#define IPCR_RD_ADDR	0x4009
#define OPCR_WR_ADDR	0x401B
#define INPUT_RD_ADDR	0x401B
#define OUT_SET_ADDR	0x401D
#define OUT_RESET_ADDR	0x401F


static char *tty_out = (char *) TBA_WR_ADDR;
static char *tty_in = (char *) RBA_RD_ADDR;
static char *tty_status = (char *) SRA_RD_ADDR;

int init_tty()
{
	*((char *) MR1A_MR2A_ADDR) = MR1A_MODE_A_REG_1_CONFIG;
	*((char *) MR1A_MR2A_ADDR) = MR2A_MODE_A_REG_2_CONFIG;
	*((char *) CSRA_WR_ADDR) = CSRA_CLK_SELECT_REG_A_CONFIG;
	*((char *) ACR_WR_ADDR) = ACR_AUX_CONTROL_REG_CONFIG;

	*((char *) CRA_WR_ADDR) = CMD_ENABLE_TX_RX;


	// Turn ON Test LED
	*((char *) OPCR_WR_ADDR) = 0x00;
	//*((char *) OUT_SET_ADDR) = 0xF0;
	*((char *) OUT_RESET_ADDR) = 0xF0;
}

int getchar(void)
{
char in;
	while (1) {
		if (*tty_status & 0x01)
			return *tty_in;

		in = (*((char *) INPUT_RD_ADDR) & 0x0f);
		if (in & 0x04) {
			putchar('0' + in);
			*((char *) OUT_SET_ADDR) = 0xF0;
		} else {
			*((char *) OUT_RESET_ADDR) = 0xF0;
		}
	}
}

int putchar(int ch)
{
	while (!(*tty_status & 0x04)) { }
	*tty_out = ch;
	return ch;
}

