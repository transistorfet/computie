
#ifndef _TERMIOS_H
#define _TERMIOS_H

typedef unsigned short tcflag_t;
typedef unsigned char cc_t;
typedef unsigned int speed_t;

#define NCCS		20	// size of cc_c array

struct termios {
	tcflag_t c_iflag;	// Input modes
	tcflag_t c_oflag;	// Output modes
	tcflag_t c_cflag;	// Control modes
	tcflag_t c_lflag;	// Local modes
	cc_t c_cc[NCCS];	// Control characters
};

// Input Modes: termios c_iflag
#define BRKINT		0x0001	// Signal interrupt on break
#define ICRNL		0x0002	// Map CR to NL on input
#define IGNBRK		0x0004	// Ignore break condition
#define IGNCR		0x0008	// Ignore CR
#define IGNPAR		0x0010	// Ignore characters with parity errors
#define INLCR		0x0020	// Map NL to CR on input
#define INPCK		0x0040	// Enable input parity check
#define ISTRIP		0x0080	// Strip character
#define IXOFF		0x0100	// Enable start/stop input control
#define IXON		0x0200	// Enable start/stop output control
#define IXANY		0x0400	// Enable any character to restart output
#define PARMRK		0x0800	// Mark parity errors

// Output Modes: termios c_oflag
#define OPOST		0x0001	// Perform output processing
#define ONLCR		0x0002	// Map NL to CR-NL on output

// Control Modes: termios c_cflag
#define CLOCAL		0x0001	// Ignore modem status lines
#define CREAD		0x0002	// Enable receiver
#define CSIZE		0x000C	// Number of bits per character
#define	CS5		0x0000	// characters are 5 bits
#define	CS6		0x0004	// characters are 6 bits
#define	CS7		0x0008	// characters are 7 bits
#define	CS8		0x000C	// characters are 8 bits
#define CSTOPB		0x0010	// Send two stop bits if set, else one
#define HUPCL		0x0020	// Hang up on last close
#define PARENB		0x0040	// Enable parity
#define PARODD		0x0080	// Odd parity if set (Even parity is default)

// Local Modes: termios c_lflag
#define ECHO		0x0001	// Enable echo
#define ECHOE		0x0002	// Echo ERASE character as error-correcting backspace
#define ECHOK		0x0004	// Echo KILL
#define ECHONL		0x0008	// Echo NL
#define ICANON		0x0010	// Canonical input (erase and kill enabled)
#define IEXTEN		0x0020	// Enable extended input character processing
#define ISIG		0x0040	// Enable signals
#define NOFLSH		0x0080	// Disable flush after interrupt or quit
#define TOSTOP		0x0100	// Send SIGTTOU for background output

// Indices into c_cc array.  Default values in parentheses
#define VEOF               0	// cc_c[VEOF] = EOF char (^D)
#define VEOL               1	// cc_c[VEOL] = EOL char (undef)
#define VERASE             2	// cc_c[VERASE] = ERASE char (^H)
#define VINTR              3	// cc_c[VINTR] = INTR char (DEL)
#define VKILL              4	// cc_c[VKILL] = KILL char (^U)
#define VMIN               5	// cc_c[VMIN] = MIN value for timer
#define VQUIT              6	// cc_c[VQUIT] = QUIT char (^\)
#define VTIME              7	// cc_c[VTIME] = TIME value for timer
#define VSUSP              8	// cc_c[VSUSP] = SUSP (^Z, ignored)
#define VSTART             9	// cc_c[VSTART] = START char (^S)
#define VSTOP             10	// cc_c[VSTOP] = STOP char (^Q)


// Values for the baud rate settings
#define B0		0x0000	// Hang up
#define B50		0x1000	// 50 baud
#define B75		0x2000	// 75 baud
#define B110		0x3000	// 110 baud
#define B134		0x4000	// 134.5 baud
#define B150		0x5000	// 150 baud
#define B200		0x6000	// 200 baud
#define B300		0x7000	// 300 baud
#define B600		0x8000	// 600 baud
#define B1200		0x9000	// 1200 baud
#define B1800		0xA000	// 1800 baud
#define B2400		0xB000	// 2400 baud
#define B4800		0xC000	// 4800 baud
#define B9600		0xD000	// 9600 baud
#define B19200		0xE000	// 19200 baud
#define B38400		0xF000	// 38400 baud

// Optional actions for tcsetattr()
#define TCSANOW            1	// Change attributes immediately
#define TCSADRAIN          2	// Change attributes when output has drained
#define TCSAFLUSH          3	// Change attributes when output has drained; also flush pending input

// Queue_selector values for tcflush()
#define TCIFLUSH           1	// Flush pending input
#define TCOFLUSH           2	// Flush both pending input and untransmitted output
#define TCIOFLUSH          3	// Flush untransmitted output

// Action values for tcflow()
#define TCOOFF             1	// Suspend output
#define TCOON              2	// Restart output
#define TCIOFF             3	// Transmit a STOP character, intended to suspend input data
#define TCION              4	// Transmit a START character, intended to restart input data


//speed_t cfgetispeed(const struct termios *);
//speed_t cfgetospeed(const struct termios *);
//int cfsetispeed(struct termios *, speed_t);
//int cfsetospeed(struct termios *, speed_t);
//int tcdrain(int);
//int tcflow(int, int);
//int tcflush(int, int);
//pid_t tcgetsid(int);
//int tcsendbreak(int, int);

int tcgetattr(int fd, struct termios *tio);
int tcsetattr(int fd, int optional_actions, const struct termios *tio);

#endif
