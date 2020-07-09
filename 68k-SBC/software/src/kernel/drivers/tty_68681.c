
#include <stdint.h>

#include <sys/stat.h>
#include <kernel/vfs.h>
#include <kernel/driver.h>

#include "../process.h"
#include "../interrupts.h"
#include "../fs/mallocfs/mallocfs.h"

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
#define CMD_ENABLE_TX_RX		0x05


// MC68681 Default Configuration Values
#define MR1A_MODE_A_REG_1_CONFIG	0b10010011	// RTS Enabled, 8 bits, No Parity
#define MR2A_MODE_A_REG_2_CONFIG	0b00010111	// Normal mode, CTS Enabled, 1 stop bit
//#define CSRA_CLK_SELECT_REG_A_CONFIG	0b10111011	// 9600 bps @ 3.6864MHz (19200 @ 7.3728 MHz)
//#define ACR_AUX_CONTROL_REG_CONFIG	0b11111000	// Set2, External Clock / 16, IRQs disabled except IP3
#define CSRA_CLK_SELECT_REG_A_CONFIG	0b11001100	// 38400 bps @ 3.6864MHz
#define ACR_AUX_CONTROL_REG_CONFIG	0b01111000	// Set1, External Clock / 16, IRQs disabled except IP3

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


#define MAX_BUFFER	128

struct circular_buffer {
	volatile char in;
	volatile char out;
	unsigned char buffer[MAX_BUFFER];
};

struct serial_channel {
	struct circular_buffer rx;
	struct circular_buffer tx;
};

static struct serial_channel channel_a;
static struct serial_channel channel_b;


extern void enter_irq();

// TODO remove after debugging
char tick = 0;


static inline void _buf_init(struct circular_buffer *cb);
static inline char _buf_is_full(struct circular_buffer *cb);
static inline char _buf_is_empty(struct circular_buffer *cb);
static inline char _buf_next_in(struct circular_buffer *cb);
static inline int _buf_get_char(struct circular_buffer *cb);
static inline int _buf_put_char(struct circular_buffer *cb, char ch);
static inline char _buf_get(struct circular_buffer *cb, char *data, char size);
static inline char _buf_put(struct circular_buffer *cb, const char *data, char size);


int tty_68681_init()
{
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
	*IMR_WR_ADDR = ISR_TIMER_CHANGE | ISR_INPUT_CHANGE | ISR_CH_A_RX_READY_FULL;

	// Flash LEDs briefly at boot
	*OPCR_WR_ADDR = 0x00;
	*OUT_SET_ADDR = 0xF0;
	*OUT_RESET_ADDR = 0xF0;

	extern struct vnode *tty_vnode;
	register_driver(DEVMAJOR_TTY, &tty_68681_driver);
	mallocfs_mknod(mallocfs_root, "tty", S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, 0, &tty_vnode);
}

int getchar(void)
{
	/*
	char in;

	while (1) {
		if (*SRA_RD_ADDR & 0x01)
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
	//printf("Status: %x\n", status);

	while (_buf_is_empty(&channel_a.rx)) {
		asm volatile("");
		//putchar('^');
	}
	return _buf_get_char(&channel_a.rx);
}

int putchar_direct(int ch)
{
	while (!(*SRA_RD_ADDR & 0x04)) { }
	*TBA_WR_ADDR = (char) ch;
	return ch;
}

int putchar(int ch)
{
	while (!(*SRA_RD_ADDR & 0x04)) { }
	*TBA_WR_ADDR = (char) ch;
	return ch;
	/*
	char data = ch;
	if (_buf_is_full(&channel_a.tx)) {
		putchar_direct('@');
	}
	_buf_put(&channel_a.tx, &data, 1);
	*IMR_WR_ADDR |= ISR_CH_A_TX_READY;
	return ch;
	*/
}


int tty_68681_open(devminor_t minor, int access)
{

}

int tty_68681_close(devminor_t minor)
{

}

int tty_68681_read(devminor_t minor, char *buffer, size_t size)
{
	int i = size;

	for (; i > 0; i--, buffer++) {
		if (_buf_is_empty(&channel_a.rx)) {
			suspend_current_proc();
			return size - i;
		}
		*buffer = getchar();
	}
	return size;
}

int tty_68681_write(devminor_t minor, const char *buffer, size_t size)
{
	for (; size > 0; size--, buffer++)
		putchar(*buffer);
	return size;
}

int tty_68681_ioctl(devminor_t minor, unsigned int request, void *argp)
{

}


// NOTE the entry to this is in syscall_entry.s
void handle_serial_irq()
{
	register char isr = *ISR_RD_ADDR;

	if (isr & ISR_CH_A_RX_READY_FULL) {
		char ch = *RBA_RD_ADDR;
		if (!_buf_is_full(&channel_a.rx)) {
			_buf_put_char(&channel_a.rx, ch);
		}
		else {
			printf("Lost: %d %d\n", channel_a.rx.in, channel_a.rx.out);
		}
		// TODO this is a hack
		resume_all_procs();
	}

	/*
	if (isr & ISR_CH_A_TX_READY) {
		char ch = _buf_get_char(&channel_a.tx);
		if (ch != -1) {
			*TBA_WR_ADDR = (char) ch;
		}
		else {
			*IMR_WR_ADDR &= ~ISR_CH_A_TX_READY;
		}
	}
	*/

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

/*
	if (isr & ISR_INPUT_CHANGE) {
// TODO commenting out because with the context switch, this code doesn't work correctly

		// Reading from the IPCR register will clear the interrupt
		uint8_t status = *IPCR_RD_ADDR;

		printf("%x\n", status);

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
		//*((uint16_t *) (current_proc_stack - CONTEX_SIZE)) |= 0x8000;
	}
*/
}



static inline void _buf_init(struct circular_buffer *cb)
{
	cb->in = 0;
	cb->out = 0;
}

static inline char _buf_is_full(struct circular_buffer *cb)
{
	return _buf_next_in(cb) == cb->out;
}

static inline char _buf_is_empty(struct circular_buffer *cb)
{
	return cb->in == cb->out;
}

static inline char _buf_next_in(struct circular_buffer *cb)
{
	return cb->in + 1 < MAX_BUFFER ? cb->in + 1 : 0;
}

static inline int _buf_get_char(struct circular_buffer *cb)
{
	register char ch;

	if (cb->out == cb->in)
		return -1;
	ch = cb->buffer[cb->out++];
	if (cb->out >= MAX_BUFFER)
		cb->out = 0;
	return ch;
}

static inline int _buf_put_char(struct circular_buffer *cb, char ch)
{
	register char next;

	next = _buf_next_in(cb);
	if (next == cb->out)
		return -1;
	cb->buffer[cb->in] = ch;
	cb->in = next;
	return ch;
}


static inline char _buf_get(struct circular_buffer *cb, char *data, char size)
{
	char i;

	for (i = 0; i < size; i++) {
		if (cb->out == cb->in)
			return i;
		data[i] = cb->buffer[cb->out++];
		if (cb->out >= MAX_BUFFER)
			cb->out = 0;
	}
	return i;
}

static inline char _buf_put(struct circular_buffer *cb, const char *data, char size)
{
	char i;
	register char next;

	for (i = 0; i < size; i++) {
		cb->buffer[cb->in] = data[i];
		next = _buf_next_in(cb);
		if (next == cb->out)
			return i;
		cb->in = next;
	}
	return i;
}

