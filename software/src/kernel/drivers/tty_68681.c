
#include <stdio.h>
#include <stdint.h>

#include <errno.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/ioc_tty.h>

#include <kernel/vfs.h>
#include <kernel/signal.h>
#include <kernel/printk.h>
#include <kernel/driver.h>
#include <kernel/syscall.h>
#include <kernel/scheduler.h>

#include "../time.h"
#include "../interrupts.h"
#include "../misc/circlebuf.h"


// Driver Definition
int tty_68681_init();
int tty_68681_open(devminor_t minor, int access);
int tty_68681_close(devminor_t minor);
int tty_68681_read(devminor_t minor, char *buffer, offset_t offset, size_t size);
int tty_68681_write(devminor_t minor, const char *buffer, offset_t offset, size_t size);
int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp);
offset_t tty_68681_seek(devminor_t minor, offset_t position, int whence, offset_t offset);

struct driver tty_68681_driver = {
	tty_68681_init,
	tty_68681_open,
	tty_68681_close,
	tty_68681_read,
	tty_68681_write,
	tty_68681_ioctl,
	tty_68681_seek,
};

void tty_68681_tx_safe_mode();
void tty_68681_set_leds(uint8_t bits);
void tty_68681_reset_leds(uint8_t bits);


// MC68681 Register Addresses
#define MR1A_MR2A_ADDR	((volatile uint8_t *) 0x700001)
#define SRA_RD_ADDR	((volatile uint8_t *) 0x700003)
#define CSRA_WR_ADDR	((volatile uint8_t *) 0x700003)
#define CRA_WR_ADDR	((volatile uint8_t *) 0x700005)
#define TBA_WR_ADDR	((volatile uint8_t *) 0x700007)
#define RBA_RD_ADDR	((volatile uint8_t *) 0x700007)

#define MR1B_MR2B_ADDR	((volatile uint8_t *) 0x700011)
#define SRB_RD_ADDR	((volatile uint8_t *) 0x700013)
#define CSRB_WR_ADDR	((volatile uint8_t *) 0x700013)
#define CRB_WR_ADDR	((volatile uint8_t *) 0x700015)
#define TBB_WR_ADDR	((volatile uint8_t *) 0x700017)
#define RBB_RD_ADDR	((volatile uint8_t *) 0x700017)

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


struct channel_ports {
	volatile uint8_t *port_config;
	volatile uint8_t *clock_config;
	volatile uint8_t *status;
	volatile uint8_t *command;
	volatile uint8_t *send;
	volatile uint8_t *recv;
	char ctsbit;
};

const struct channel_ports channel_a_ports = {
	MR1A_MR2A_ADDR,
	CSRA_WR_ADDR,
	SRA_RD_ADDR,
	CRA_WR_ADDR,
	TBA_WR_ADDR,
	RBA_RD_ADDR,
	0x01,
};

const struct channel_ports channel_b_ports = {
	MR1B_MR2B_ADDR,
	CSRB_WR_ADDR,
	SRB_RD_ADDR,
	CRB_WR_ADDR,
	TBB_WR_ADDR,
	RBB_RD_ADDR,
	0x02,
};


struct serial_channel {
	struct circular_buffer rx;
	struct circular_buffer tx;
	const struct channel_ports *ports;
	int pgid;
};

static struct serial_channel channel_a;
static struct serial_channel channel_b;


// TODO remove after debugging
char tick = 0;

extern void enter_irq();

static inline void config_serial_channel(struct serial_channel *channel)
{
	channel->pgid = 0;

	// Reset channel A serial port
	*channel->ports->command = CMD_RESET_MR;
	asm volatile("nop\n");
	*channel->ports->command = CMD_RESET_TX;

	// Configure channel A serial port
	*channel->ports->port_config = MR1A_MODE_A_REG_1_CONFIG;
	*channel->ports->port_config = MR2A_MODE_A_REG_2_CONFIG;
	*channel->ports->clock_config = CSRA_CLK_SELECT_REG_A_CONFIG;

	// Enable the channel A serial port
	*channel->ports->command = CMD_ENABLE_TX_RX;
}


static inline struct serial_channel *from_minor_dev(device_t minor)
{
	switch (minor) {
		case 0:
			return &channel_a;
		case 1:
			return &channel_b;
		default:
			return NULL;
	}
}

static inline int getchar_buffered(struct serial_channel *channel)
{
	// Assert CTS
	*OUT_SET_ADDR = channel->ports->ctsbit;

	//putchar_direct('#');
	while (_buf_is_empty(&channel->rx)) {
		asm volatile("");
		//putchar_buffered('^');
	}
	return _buf_get_char(&channel->rx);
}


static inline int putchar_direct(struct serial_channel *channel, int ch)
{
	*channel->ports->command = CMD_ENABLE_TX;
	while (!(*channel->ports->status & SR_TX_READY)) { }
	*channel->ports->send = (char) ch;
	return ch;
}

static inline int putchar_buffered(struct serial_channel *channel, int ch)
{
	// TODO this timelimit is because of an issue on boot where it will lock up before interrupts are enabled because the buffer is full
	for (int i = 0; _buf_is_full(&channel->tx) && i < 10000; i++) {
		asm volatile("");
	}

	_buf_put_char(&channel->tx, ch);

	// Enable the channel A transmitter
	*channel->ports->command = CMD_ENABLE_TX;

	return ch;
}

//int putchar(int ch) { return putchar_buffered(&channel_a, ch); }

 static inline void handle_channel_io(register char isr, struct serial_channel *channel)
{
	register char status = *channel->ports->status;
	register char rx_ready = channel == &channel_a ? ISR_CH_A_RX_READY_FULL : ISR_CH_B_RX_READY_FULL;
	register char tx_ready = channel == &channel_a ? ISR_CH_A_TX_READY : ISR_CH_B_TX_READY;

	if (status & SR_RX_FULL) {
		// De-Assert CTS
		*OUT_RESET_ADDR = channel->ports->ctsbit;
	}

	if (isr & rx_ready) {
		while (*channel->ports->status & SR_RX_READY) {
			if (!_buf_is_full(&channel->rx)) {
				char ch = *channel->ports->recv;
				// TODO this is a temporary hack to get ^C -> SIGINT, but it's totally wrong on so many levels
				if (ch == 0x03 && channel->pgid) {
					send_signal_process_group(channel->pgid, SIGINT);
				}
				//if (ch == 0x04 && channel->pgid) {
				//	send_signal(channel->pgid, SIGCONT);
				//}
				_buf_put_char(&channel->rx, ch);
			}
			else {
				// De-Assert CTS
				*OUT_RESET_ADDR = channel->ports->ctsbit;
				break;
			}
		}

		resume_blocked_procs(SYS_READ, NULL, DEVNUM(DEVMAJOR_TTY, channel == &channel_a ? 0 : 1));
	}


	if (isr & tx_ready) {
		int ch = _buf_get_char(&channel->tx);
		if (ch != -1) {
			*channel->ports->send = (char) ch;
		}
		else {
			// Disable the channel A transmitter if empty
			if (status & SR_TX_EMPTY)
				*channel->ports->command = CMD_DISABLE_TX;
		}
	}
}

// NOTE the entry to this is in syscall_entry.s
void handle_serial_irq()
{
	register char isr = *ISR_RD_ADDR;

	backup_current_proc();

	handle_channel_io(isr, &channel_a);

	handle_channel_io(isr, &channel_b);

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

		adjust_system_time(71111);
		check_timers();

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

		*/
	}
}



#define PRINTK_BUFFER	128

int vprintk(int direct, const char *fmt, va_list args)
{
	int i;
	char buffer[PRINTK_BUFFER];
	int (*put)(struct serial_channel *, int) = direct ? putchar_direct : putchar_buffered;

	vsnprintf(buffer, PRINTK_BUFFER, fmt, args);
	for (i = 0; i < PRINTK_BUFFER && buffer[i]; i++)
		//putchar_direct(&channel_a, buffer[i]);
		//putchar_buffered(&channel_a, buffer[i]);
		put(&channel_a, buffer[i]);
	return i;
}

int printk(const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vprintk(0, fmt, args);
	va_end(args);

	return ret;
}

int printk_safe(const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vprintk(1, fmt, args);
	va_end(args);

	return ret;
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

	*IMR_WR_ADDR = 0;
	*CRA_WR_ADDR = CMD_ENABLE_TX_RX;

	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;

	// Assert CTS
	*OUT_SET_ADDR = 0x01;

	/*
	// Enable interrupts
	set_interrupt(TTY_INT_VECTOR, enter_irq);
	*IVR_WR_ADDR = TTY_INT_VECTOR;
	*IMR_WR_ADDR = ISR_CH_A_RX_READY_FULL | ISR_CH_A_TX_READY;

	ENABLE_INTS();
	*/
}

void tty_68681_normal_mode()
{
	// Configure baud rate set and clock source
	*ACR_WR_ADDR = ACR_AUX_CONTROL_REG_CONFIG;

	config_serial_channel(&channel_a);
	config_serial_channel(&channel_b);

	// Configure timer
	*CTUR_WR_ADDR = 0x20;
	*CTLR_WR_ADDR = 0x00;

	// Enable interrupts
	set_interrupt(TTY_INT_VECTOR, enter_irq);
	*IVR_WR_ADDR = TTY_INT_VECTOR;
	*IMR_WR_ADDR = ISR_TIMER_CHANGE | ISR_INPUT_CHANGE | ISR_CH_A_RX_READY_FULL | ISR_CH_A_TX_READY;

	// Flash LEDs briefly at boot
	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;
	*OUT_RESET_ADDR = 0xF0;

	// Assert CTS on both channels
	*OUT_SET_ADDR = 0x03;
}


void tty_68681_preinit()
{
	_buf_init(&channel_a.rx);
	_buf_init(&channel_a.tx);
	channel_a.ports = &channel_a_ports;

	_buf_init(&channel_b.rx);
	_buf_init(&channel_b.tx);
	channel_b.ports = &channel_b_ports;

	tty_68681_tx_safe_mode();
}

int tty_68681_init()
{
	tty_68681_normal_mode();

	register_driver(DEVMAJOR_TTY, &tty_68681_driver);
}

int tty_68681_open(devminor_t minor, int access)
{
	return 0;
}

int tty_68681_close(devminor_t minor)
{
	return 0;
}

int tty_68681_read(devminor_t minor, char *buffer, offset_t offset, size_t size)
{
	int i = size;
	struct serial_channel *channel = from_minor_dev(minor);

	if (!channel)
		return ENODEV;

	for (; i > 0; i--, buffer++) {
		if (_buf_is_empty(&channel->rx)) {
			// Suspend the process only if we haven't read any data yet
			if (size == i)
				suspend_current_proc();
			return size - i;
		}
		*buffer = getchar_buffered(channel);
	}
	return size;
}

int tty_68681_write(devminor_t minor, const char *buffer, offset_t offset, size_t size)
{
	int i = size;
	struct serial_channel *channel = from_minor_dev(minor);

	if (!channel)
		return ENODEV;

	// TODO with this method, each write's size must always be smaller than buffer size
	if (_buf_free_space(&channel->tx) < size) {
		suspend_current_proc();
		return 0;
	}

	for (; i > 0; i--, buffer++)
		//putchar_indirect(channel, *buffer);
		putchar_buffered(channel, *buffer);
	return size;
}

int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp)
{
	switch (request) {
		case TIOCSPGRP: {
			struct serial_channel *channel = from_minor_dev(minor);
			channel->pgid = *((int *) argp);
			return 0;
		}
		default:
			break;
	}
	return -1;
}

offset_t tty_68681_seek(devminor_t minor, offset_t position, int whence, offset_t offset)
{
	return -1;
}


void tty_68681_set_leds(uint8_t bits)
{
	*OUT_SET_ADDR = (bits << 4);
}

void tty_68681_reset_leds(uint8_t bits)
{
	*OUT_RESET_ADDR = (bits << 4);
}


