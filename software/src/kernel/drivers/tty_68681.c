
#include <stdio.h>
#include <stdint.h>

#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/printk.h>
#include <kernel/driver.h>

#include "../process.h"
#include "../interrupts.h"
#include "../misc/circlebuf.h"


// Driver Definition
int tty_68681_init();
int tty_68681_open(devminor_t minor, int access);
int tty_68681_close(devminor_t minor);
int tty_68681_read(devminor_t minor, char *buffer, size_t size);
int tty_68681_write(devminor_t minor, const char *buffer, size_t size);
int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp);

struct driver tty_68681_driver = {
	tty_68681_init,
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
#define CMD_RESET_MR			0x10
#define CMD_RESET_RX			0x20
#define CMD_RESET_TX			0x30
#define CMD_RESET_ERROR			0x40
#define CMD_ENABLE_TX_RX		0x05
#define CMD_ENABLE_RX			0x01
#define CMD_DISABLE_RX			0x02
#define CMD_ENABLE_TX			0x04
#define CMD_DISABLE_TX			0x08
#define CMD_START_BREAK			0x60
#define CMD_STOP_BREAK			0x70


// MC68681 Default Configuration Values
#define MR1A_MODE_A_REG_1_CONFIG	0b10010011	// RxRTS Enabled, 8 bits, No Parity
#define MR2A_MODE_A_REG_2_CONFIG	0b00010111	// Normal mode, CTS Enabled, 1 stop bit
//#define CSRA_CLK_SELECT_REG_A_CONFIG	0b10111011	// 9600 bps @ 3.6864MHz (19200 @ 7.3728 MHz)
//#define ACR_AUX_CONTROL_REG_CONFIG	0b11111000	// Set2, External Clock / 16, IRQs disabled except IP3
#define CSRA_CLK_SELECT_REG_A_CONFIG	0b11001100	// 38400 bps @ 3.6864MHz
#define ACR_AUX_CONTROL_REG_CONFIG	0b01111000	// Set1, External Clock / 16, IRQs disabled except IP3


// Status Register Bits (SRA/SRB)
#define SR_RECEIVED_BREAK		0x80
#define SR_FRAMING_ERROR		0x40
#define SR_PARITY_ERROR			0x20
#define SR_OVERRUN_ERROR		0x10
#define SR_TX_EMPTY			0x08
#define SR_TX_READY			0x04
#define SR_RX_FULL			0x02
#define SR_RX_READY			0x01


// Interrupt Status/Mask Bits (ISR/IVR)
#define ISR_INPUT_CHANGE		0x80
#define ISR_CH_B_BREAK_CHANGE		0x40
#define ISR_CH_B_RX_READY_FULL		0x20
#define ISR_CH_B_TX_READY		0x10
#define ISR_TIMER_CHANGE		0x08
#define ISR_CH_A_BREAK_CHANGE		0x04
#define ISR_CH_A_RX_READY_FULL		0x02
#define ISR_CH_A_TX_READY		0x01


#define TTY_INT_VECTOR			IV_USER_VECTORS




struct serial_channel {
	struct circular_buffer rx;
	struct circular_buffer tx;
};

struct vnode;

static struct vnode *tty_vnode;
static struct serial_channel channel_a;
static struct serial_channel channel_b;


// TODO remove after debugging
char tick = 0;

extern void enter_irq();

int tty_68681_init()
{
	*CRA_WR_ADDR = CMD_RESET_MR;
	asm volatile("nop\n");
	*CRA_WR_ADDR = CMD_RESET_TX;

	// Configure channel A serial port
	*MR1A_MR2A_ADDR = MR1A_MODE_A_REG_1_CONFIG;
	*MR1A_MR2A_ADDR = MR2A_MODE_A_REG_2_CONFIG;
	*CSRA_WR_ADDR = CSRA_CLK_SELECT_REG_A_CONFIG;
	*ACR_WR_ADDR = ACR_AUX_CONTROL_REG_CONFIG;

	// Enable the channel A serial port
	*CRA_WR_ADDR = CMD_ENABLE_TX_RX;

	_buf_init(&channel_a.rx);
	_buf_init(&channel_a.tx);

	// Configure timer
	*CTUR_WR_ADDR = 0x1F;
	*CTLR_WR_ADDR = 0xFF;

	// Enable interrupts
	set_interrupt(TTY_INT_VECTOR, enter_irq);
	*IVR_WR_ADDR = TTY_INT_VECTOR;
	*IMR_WR_ADDR = ISR_TIMER_CHANGE | ISR_INPUT_CHANGE | ISR_CH_A_RX_READY_FULL | ISR_CH_A_TX_READY;

	// Flash LEDs briefly at boot
	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;
	*OUT_RESET_ADDR = 0xF0;

	// Assert CTS
	*OUT_SET_ADDR = 0x01;

	register_driver(DEVMAJOR_TTY, &tty_68681_driver);
	vfs_mknod(NULL, "tty", S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, 0, SU_UID, &tty_vnode);

	*((char *) 0x201d) = 0x00;
}

void tty_68681_tx_safe_mode()
{
	DISABLE_INTS();

	*CRA_WR_ADDR = CMD_RESET_MR;

	// Configure channel A serial port
	*MR1A_MR2A_ADDR = MR1A_MODE_A_REG_1_CONFIG;
	*MR1A_MR2A_ADDR = MR2A_MODE_A_REG_2_CONFIG;
	*CSRA_WR_ADDR = CSRA_CLK_SELECT_REG_A_CONFIG;
	*ACR_WR_ADDR = ACR_AUX_CONTROL_REG_CONFIG;

	*CRA_WR_ADDR = CMD_ENABLE_TX_RX;

	/*
	// Enable interrupts
	set_interrupt(TTY_INT_VECTOR, enter_irq);
	*IVR_WR_ADDR = TTY_INT_VECTOR;
	*IMR_WR_ADDR = ISR_CH_A_RX_READY_FULL | ISR_CH_A_TX_READY;

	ENABLE_INTS();
	*/
}

void tty_68681_set_leds(uint8_t bits)
{
	*OUT_SET_ADDR = (bits << 4);
}

void tty_68681_reset_leds(uint8_t bits)
{
	*OUT_RESET_ADDR = (bits << 4);
}


int getchar_buffered(void)
{
	/*
	char in;

	while (1) {
		if (*SRA_RD_ADDR & SR_RX_READY)
			return *RBA_RD_ADDR;

		// Debugging - Set all LEDs on if an input is active
		//in = (*INPUT_RD_ADDR & 0x3f);
		//if (in & 0x18) {
		//	*OUT_SET_ADDR = 0xF0;
		//} else {
		//	*OUT_RESET_ADDR = 0xF0;
		//}

		// Debugging - Set LEDs on timer
		//if (*ISR_RD_ADDR & ISR_TIMER_CHANGE) {
		//	tick = !tick;
		//	*OUT_SET_ADDR = tick;
		//	*OUT_RESET_ADDR = !tick;
		//}
	}
	*/

	//uint16_t status;
	//asm("move.w	%%sr, %0\n" : "=g" (status));
	//printk("Status: %x\n", status);

	// Assert CTS
	*OUT_SET_ADDR = 0x01;

	//putchar_direct('#');
	while (_buf_is_empty(&channel_a.rx)) {
		asm volatile("");
		//putchar_buffered('^');
	}
	return _buf_get_char(&channel_a.rx);
}


int putchar_direct(int ch)
{
	*CRA_WR_ADDR = CMD_ENABLE_TX;
	while (!(*SRA_RD_ADDR & SR_TX_READY)) { }
	*TBA_WR_ADDR = (char) ch;
	return ch;
}

int putchar_buffered(int ch)
{
	//tty_68681_set_leds(0x02);

	// TODO this timelimit is because of an issue on boot where it will lock up before interrupts are enabled because the buffer is full
	for (int i = 0; _buf_is_full(&channel_a.tx) && i < 10000; i++) {
		asm volatile("");
	}

	//tty_68681_reset_leds(0x02);

	_buf_put_char(&channel_a.tx, ch);

	// Enable the channel A transmitter
	*CRA_WR_ADDR = CMD_ENABLE_TX;

	return ch;
}

//int putchar(int ch) { return putchar_buffered(ch); }


#define PRINTK_BUFFER	128

int vprintk(const char *fmt, va_list args)
{
	int i;
	char buffer[PRINTK_BUFFER];

	vsnprintf(buffer, PRINTK_BUFFER, fmt, args);
	for (i = 0; i < PRINTK_BUFFER && buffer[i]; i++)
		//putchar_direct(buffer[i]);
		putchar_buffered(buffer[i]);
	return i;
}

int printk(const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vprintk(fmt, args);
	va_end(args);

	return ret;
}


int tty_68681_open(devminor_t minor, int access)
{
	return 0;
}

int tty_68681_close(devminor_t minor)
{
	return 0;
}

int tty_68681_read(devminor_t minor, char *buffer, size_t size)
{
	int i = size;

	for (; i > 0; i--, buffer++) {
		if (_buf_is_empty(&channel_a.rx)) {
			// Suspend the process only if we haven't read any data yet
			if (size == i) {
				//putchar_direct('S');
				suspend_current_proc();
			}
			return size - i;
		}
		*buffer = getchar_buffered();
	}
	return size;
}

int tty_68681_write(devminor_t minor, const char *buffer, size_t size)
{
	int i = size;

	// TODO with this method, each write's size must always be smaller than buffer size
	if (_buf_free_space(&channel_a.tx) < size) {
		//putchar_direct('S');
		suspend_current_proc();
		return 0;
	}

	for (; i > 0; i--, buffer++)
		//putchar_indirect(*buffer);
		putchar_buffered(*buffer);
	return size;
}

int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp)
{
	return -1;
}


// NOTE the entry to this is in syscall_entry.s
void handle_serial_irq()
{
	register char isr = *ISR_RD_ADDR;
	register char status = *SRA_RD_ADDR;

	//*OUT_SET_ADDR = 0x10;

	if (status & SR_RX_FULL) {
		// De-Assert CTS
		*OUT_RESET_ADDR = 0x01;
	}

	// TODO this is for debugging
	if (status & (SR_OVERRUN_ERROR | SR_PARITY_ERROR | SR_FRAMING_ERROR)) {
		printk("Game Over");
		asm("stop #0x2700\n");
	}

	if (isr & ISR_CH_A_RX_READY_FULL) {
		while (*SRA_RD_ADDR & SR_RX_READY) {
			if (!_buf_is_full(&channel_a.rx)) {
				char ch = *RBA_RD_ADDR;
				_buf_put_char(&channel_a.rx, ch);
			}
			else {
				// De-Assert CTS
				*OUT_RESET_ADDR = 0x01;
				//printk("Lost: %d %d\n", channel_a.rx.in, channel_a.rx.out);
				break;
			}
		}

		resume_blocked_procs(SYS_READ, tty_vnode);
	}


	if (isr & ISR_CH_A_TX_READY) {
		int ch = _buf_get_char(&channel_a.tx);
		if (ch != -1) {
			*TBA_WR_ADDR = (char) ch;
		}
		else {
			if (status & SR_TX_EMPTY)
			// Disable the channel A transmitter
			*CRA_WR_ADDR = CMD_DISABLE_TX;
			//*TBA_WR_ADDR = '*';
		}
	}

	if (isr & ISR_TIMER_CHANGE) {
		// Clear the interrupt bit by reading the stop address
		register char reset = *STOP_RD_ADDR;

		if (tick) {
			tick = 0;
			*OUT_SET_ADDR = 0x80;
		} else {
			tick = 1;
			*OUT_RESET_ADDR = 0x80;
		}

		// Schedule a new process
		schedule();
	}

	if (isr & ISR_INPUT_CHANGE) {
		// Reading from the IPCR register will clear the interrupt
		uint8_t status = *IPCR_RD_ADDR;

		tty_68681_tx_safe_mode();
		asm(
		"move.l	#0, %a0\n"
		"movec	%a0, %vbr\n"
		"move.l	#0x20, %a0\n"
		"jmp	(%a0)\n"
		);

		//if (status & 0x03) {
		//	putchar_buffered('!');
		//	TRACE_ON();
		//}
		//else if (!(status & 0x03)) {
		//	TRACE_OFF();
		//}

		/*
		// TODO commenting out because with the context switch, this code doesn't work correctly
		printk("%x\n", status);

		if (*INPUT_RD_ADDR & 0x08) {
			asm(
			"move.w	(%a5), %d0\n"
			"not.w	%d0\n"
			"and.w	#0x8000, %d0\n"
			"or.w	%d0, (%a5)\n"
			);
		}
		//TRACE_ON();

		//extern char *current_proc_stack;
		// *((uint16_t *) (current_proc_stack - CONTEX_SIZE)) |= 0x8000;
		*/
	}

	//*OUT_RESET_ADDR = 0x10;
}


