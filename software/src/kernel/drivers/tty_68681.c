
#include <stdio.h>
#include <stdint.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/ioc_tty.h>

#include <asm/macros.h>
#include <kernel/bh.h>
#include <kernel/vfs.h>
#include <kernel/time.h>
#include <kernel/signal.h>
#include <kernel/printk.h>
#include <kernel/driver.h>
#include <kernel/syscall.h>
#include <kernel/scheduler.h>

#include "../interrupts.h"
#include "../proc/timer.h"
#include "../misc/circlebuf.h"


// Driver Definition
int tty_68681_init();
int tty_68681_open(devminor_t minor, int access);
int tty_68681_close(devminor_t minor);
int tty_68681_read(devminor_t minor, char *buffer, offset_t offset, size_t size);
int tty_68681_write(devminor_t minor, const char *buffer, offset_t offset, size_t size);
int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp, uid_t uid);
int tty_68681_poll(devminor_t minor, int events);
offset_t tty_68681_seek(devminor_t minor, offset_t position, int whence, offset_t offset);

struct driver tty_68681_driver = {
	tty_68681_init,
	tty_68681_open,
	tty_68681_close,
	tty_68681_read,
	tty_68681_write,
	tty_68681_ioctl,
	tty_68681_poll,
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
#define MR2A_MODE_A_REG_2_CONFIG	0b00000111	// Normal mode, CTS Disabled, 1 stop bit
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

#define ISR_S_BREAK_CHANGE		0x04
#define ISR_S_RX_READY_FULL		0x02
#define ISR_S_TX_READY			0x01


#define TTY_INT_VECTOR			IV_USER_VECTORS

#define CH_A				0
#define CH_B				1

#define ASSERT_CTS(channel)		(*OUT_SET_ADDR = (channel)->ports->ctsbit)
#define DEASSERT_CTS(channel)		(*OUT_RESET_ADDR = (channel)->ports->ctsbit)


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
	int opens;
	int open_mode;
	char rx_ready;
	char bh_num;
};

static char tick = 0;
static char handle_timer = 0;
static struct serial_channel channels[2];

extern void enter_irq();

static inline void config_serial_channel(struct serial_channel *channel)
{
	channel->opens = 0;
	channel->open_mode = 0;
	channel->rx_ready = 0;
	channel->bh_num = BH_TTY;

	// Reset channel A serial port
	*channel->ports->command = CMD_RESET_MR;
	NOP();
	*channel->ports->command = CMD_RESET_TX;
	NOP();

	// Configure serial port
	*channel->ports->port_config = MR1A_MODE_A_REG_1_CONFIG;
	*channel->ports->port_config = MR2A_MODE_A_REG_2_CONFIG;
	*channel->ports->clock_config = CSRA_CLK_SELECT_REG_A_CONFIG;

	// Enable the serial port
	*channel->ports->command = CMD_ENABLE_TX_RX;
}


static inline struct serial_channel *from_minor_dev(device_t minor)
{
	switch (minor) {
		case 0:
			return &channels[CH_A];
		case 1:
			return &channels[CH_B];
		default:
			return NULL;
	}
}

static inline int getchar_buffered(struct serial_channel *channel)
{
	short saved_status;
	LOCK(saved_status);

	// TODO maybe this should return some kind of error, like -1
	while (_buf_is_empty(&channel->rx)) {
		asm volatile("");
		//putchar_buffered('^');
	}
	unsigned char ch = _buf_get_char(&channel->rx);

	if (_buf_free_space(&channel->rx) > 100)
		ASSERT_CTS(channel);

	UNLOCK(saved_status);
	return ch;
}


static inline int putchar_direct(struct serial_channel *channel, int ch)
{
	short saved_status;
	LOCK(saved_status);

	*channel->ports->command = CMD_ENABLE_TX;
	while (!(*channel->ports->status & SR_TX_READY)) { }
	*channel->ports->send = (unsigned char) ch;

	UNLOCK(saved_status);
	return ch;
}

static inline int putchar_buffered(struct serial_channel *channel, int ch)
{
	short saved_status;
	LOCK(saved_status);

	// TODO this timelimit is because of an issue on boot where it will lock up before interrupts are enabled because the buffer is full
	for (int i = 0; _buf_is_full(&channel->tx) && i < 10000; i++) {
		asm volatile("");
	}

	_buf_put_char(&channel->tx, ch);

	// Enable the channel A transmitter
	*channel->ports->command = CMD_ENABLE_TX;

	UNLOCK(saved_status);
	return ch;
}

//int putchar(int ch) { return putchar_buffered(&channels[CH_A], ch); }

static void tty_68681_process_input(void *_unused)
{
	if (handle_timer) {
		handle_timer = 0;
		check_timers();
	}

	for (char i = 0; i < 2; i++) {
		if (channels[i].rx_ready) {
			channels[i].rx_ready = 0;
			resume_blocked_procs(VFS_POLL_READ, NULL, DEVNUM(DEVMAJOR_TTY68681, i));

			//request_bh_run(BH_TTY);
			request_bh_run(channels[i].bh_num);

			// TODO this isn't needed now, since the assert in getchar handle this, but once we have the tty subsystem this might be needed
			//if (_buf_is_empty(&channels[i].rx))
			//	ASSERT_CTS(&channels[i]);
		}
	}
}

static inline void handle_channel_io(register char isr, register devminor_t minor)
{
	// The ISR bits for channel A and B are the same but separated by 4 bits in the same register
	if (minor == CH_B)
		isr >>= 4;

	if (isr & ISR_S_RX_READY_FULL) {
		while (*channels[minor].ports->status & SR_RX_READY) {
			if (_buf_is_full(&channels[minor].rx))
				break;
			_buf_put_char(&channels[minor].rx, *channels[minor].ports->recv);
		}

		if (_buf_free_space(&channels[minor].rx) < 10)
			DEASSERT_CTS(&channels[minor]);

		channels[minor].rx_ready = 1;
		request_bh_run(BH_TTY68681);
		//resume_blocked_procs(VFS_POLL_READ, NULL, DEVNUM(DEVMAJOR_TTY68681, channel == &channels[CH_A] ? 0 : 1));
	}


	if (isr & ISR_S_TX_READY) {
		int ch = _buf_get_char(&channels[minor].tx);
		if (ch != -1)
			*channels[minor].ports->send = (unsigned char) ch;
		else {
			// Disable the channel A transmitter if empty
			if (*channels[minor].ports->status & SR_TX_EMPTY)
				*channels[minor].ports->command = CMD_DISABLE_TX;
		}
	}
}

// NOTE the entry to this is in syscall_entry.s
void handle_serial_irq()
{
	// TODO this is for debugging to tell me when the handler exits
	*OUT_SET_ADDR = 0x08;

	register char isr = *ISR_RD_ADDR;

	for (char i = 0; i < 2; i++)
		handle_channel_io(isr, i);


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
		request_reschedule();

		handle_timer = 1;
		request_bh_run(BH_TTY68681);
	}

	/*
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

	}
	*/

	// TODO this is for debugging to tell me when the handler exits
	*OUT_RESET_ADDR = 0x08;
}



#define PRINTK_BUFFER	128

int vprintk(int buffered, const char *fmt, va_list args)
{
	int i;
	char buffer[PRINTK_BUFFER];
	int (*put)(struct serial_channel *, int) = buffered ? putchar_buffered : putchar_direct;

	vsnprintf(buffer, PRINTK_BUFFER, fmt, args);
	for (i = 0; i < PRINTK_BUFFER && buffer[i]; i++)
		//putchar_direct(&channels[CH_A], buffer[i]);
		//putchar_buffered(&channels[CH_A], buffer[i]);
		put(&channels[CH_A], buffer[i]);
	return i;
}

int printk(const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vprintk(1, fmt, args);
	va_end(args);

	return ret;
}

int printk_safe(const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vprintk(0, fmt, args);
	va_end(args);

	return ret;
}

__attribute__((noreturn)) void panic(const char *fmt, ...)
{
	va_list args;

	tty_68681_tx_safe_mode();

	va_start(args, fmt);
	vprintk(0, fmt, args);
	va_end(args);

	HALT();
	__builtin_unreachable();
}

void prepare_for_panic()
{
	tty_68681_tx_safe_mode();
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

	// This slight delay prevents garbled output being sent during the welcome message
	inline_delay(10);

	ASSERT_CTS(&channels[CH_A]);
}

void tty_68681_normal_mode()
{
	// Configure baud rate set and clock source
	*ACR_WR_ADDR = ACR_AUX_CONTROL_REG_CONFIG;

	config_serial_channel(&channels[CH_A]);
	config_serial_channel(&channels[CH_B]);
	channels[CH_B].bh_num = BH_SLIP;

	// Configure timer
	*CTUR_WR_ADDR = 0x20;
	*CTLR_WR_ADDR = 0x00;

	// Enable interrupts
	set_interrupt(TTY_INT_VECTOR, enter_irq);
	*IVR_WR_ADDR = TTY_INT_VECTOR;
	*IMR_WR_ADDR = ISR_TIMER_CHANGE | ISR_INPUT_CHANGE | ISR_CH_A_RX_READY_FULL | ISR_CH_A_TX_READY | ISR_CH_B_RX_READY_FULL | ISR_CH_B_TX_READY;

	// Flash LEDs briefly at boot
	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;
	*OUT_RESET_ADDR = 0xF0;

	// This slight delay prevents garbled output being sent during the welcome message
	inline_delay(10);

	ASSERT_CTS(&channels[CH_A]);
	ASSERT_CTS(&channels[CH_B]);
}


void tty_68681_preinit()
{
	_buf_init(&channels[CH_A].rx, 0);
	_buf_init(&channels[CH_A].tx, 0);
	channels[CH_A].ports = &channel_a_ports;

	_buf_init(&channels[CH_B].rx, 0);
	_buf_init(&channels[CH_B].tx, 0);
	channels[CH_B].ports = &channel_b_ports;

	tty_68681_tx_safe_mode();
}

int tty_68681_init()
{
	tty_68681_normal_mode();

	register_driver(DEVMAJOR_TTY68681, &tty_68681_driver);
	register_bh(BH_TTY68681, tty_68681_process_input, NULL);
	enable_bh(BH_TTY68681);
}

int tty_68681_open(devminor_t minor, int mode)
{
	struct serial_channel *channel = from_minor_dev(minor);

	if (!channel)
		return ENODEV;
	if (!mode)
		return EBUSY;
	if (channel->opens != 0 && ((channel->open_mode & O_EXCL) || (mode & O_EXCL)))
		return EBUSY;
	channel->opens++;
	channel->open_mode = mode;
	return 0;
}

int tty_68681_close(devminor_t minor)
{
	struct serial_channel *channel = from_minor_dev(minor);

	if (!channel)
		return ENODEV;
	channel->opens--;
	channel->open_mode = 0;
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
			if (size == i && !(channel->open_mode & O_NONBLOCK))
				suspend_current_syscall(VFS_POLL_READ);
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
		if (!(channel->open_mode & O_NONBLOCK))
			suspend_current_syscall(VFS_POLL_WRITE);
		return 0;
	}

	for (; i > 0; i--, buffer++)
		//putchar_indirect(channel, *buffer);
		putchar_buffered(channel, *buffer);
	return size;
}

int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp, uid_t uid)
{
	struct serial_channel *channel = from_minor_dev(minor);

	if (!channel)
		return ENODEV;

	switch (request) {
		case TSETLEDS: {
			int leds = *((int *) argp) & 0x03;
			tty_68681_reset_leds(~leds);
			tty_68681_set_leds(leds);
			return 0;
		}
		default:
			break;
	}
	return -1;
}

int tty_68681_poll(devminor_t minor, int events)
{
	int revents = 0;
	struct serial_channel *channel = from_minor_dev(minor);

	if ((events & VFS_POLL_READ) && channel->opens > 0 && !_buf_is_empty(&channel->rx))
		revents |= VFS_POLL_READ;
	if ((events & VFS_POLL_WRITE) && channel->opens > 0 && !_buf_is_full(&channel->tx))
		revents |= VFS_POLL_WRITE;

	return revents;
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


