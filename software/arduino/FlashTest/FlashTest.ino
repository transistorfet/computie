
#define SERIAL_SPEED	115200

#define MEM_TEST_HADDR	0x00

#define MEM_TEST_START	0x00000
#define MEM_TEST_SIZE	32700

#define MEM_XADDR_PORT	PORTF
#define MEM_XADDR_PIN	PINF
#define MEM_XADDR_DDR	DDRF

#define MEM_HADDR_PORT	PORTC
#define MEM_HADDR_PIN	PINC
#define MEM_HADDR_DDR	DDRC

#define MEM_LADDR_PORT	PORTL
#define MEM_LADDR_PIN	PINL
#define MEM_LADDR_DDR	DDRL

#define MEM_LDATA_PORT	PORTA
#define MEM_LDATA_PIN	PINA
#define MEM_LDATA_DDR	DDRA

//#define MEM_HDATA_PORT	PORTK
//#define MEM_HDATA_PIN	PINK
//#define MEM_HDATA_DDR	DDRK

#define MEM_CE		50 // PB3
#define MEM_OE		51 // PB2
#define MEM_WE		53 // PB0

byte bus_request = 0;



#undef INLINE_NOP
#define INLINE_NOP		__asm__("nop\n\t");

#define BUS_DEVICE	0
#define BUS_CONTROLLER	1

byte bus_mode = BUS_DEVICE;

#define TTY_COMMAND	0
#define TTY_PASS	1

byte tty_mode = TTY_COMMAND;

void pciSetup(byte pin)
{
	*digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
	PCIFR |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

void pciDisable(byte pin)
{
	PCICR &= ~(bit (digitalPinToPCICRbit(pin))); // disable interrupt for the group
}

void set_bus_mode_controller()
{
	pciDisable(MEM_CE);

	// Controls
	pinMode(MEM_CE, OUTPUT);
	pinMode(MEM_OE, OUTPUT);
	pinMode(MEM_WE, OUTPUT);
	digitalWrite(MEM_CE, 1);
	digitalWrite(MEM_OE, 1);
	digitalWrite(MEM_WE, 1);

	// Address Bus
	MEM_XADDR_PORT = 0x00;	// A16-A18 (for bit banged)
	MEM_XADDR_DDR = 0xFF;
	MEM_HADDR_PORT = 0x00;	// A8 - A15 (for bit banged)
	MEM_HADDR_DDR = 0xFF;
	MEM_LADDR_PORT = 0x00;	// A0 - A7 (for bit banged)
	MEM_LADDR_DDR = 0xFF;

	// Data Bus
	MEM_LDATA_PORT = 0x00;	// D0 - D7
	MEM_LDATA_DDR = 0x00;

	tty_mode = TTY_COMMAND;
	bus_mode = BUS_CONTROLLER;
}

void set_bus_mode_device()
{
	// Controls
	pinMode(MEM_CE, INPUT);
	pinMode(MEM_OE, INPUT);
	pinMode(MEM_WE, INPUT);

	// Address Bus
	MEM_XADDR_PORT = 0x00;	// A16 - A18 (for bit banged)
	MEM_XADDR_DDR = 0x00;
	MEM_HADDR_PORT = 0x00;	// A8 - A15 (for bit banged)
	MEM_HADDR_DDR = 0x00;
	MEM_LADDR_PORT = 0x00;	// A0 - A7 (for bit banged)
	MEM_LADDR_DDR = 0x00;

	// Data Bus
	MEM_LDATA_PORT = 0x00;	// D0 - D7
	MEM_LDATA_DDR = 0x00;

	pciSetup(MEM_CE);
	bus_mode = BUS_DEVICE;
}

/******************************
 * Serial Communications Code *
 ******************************/

#define SERIAL_SIZE	64

char serial_read_tail = 0;
char serial_read_head = 0;
char serial_avail = 0;
char serial_rb[SERIAL_SIZE];
char serial_write_head = 0;
char serial_write_tail = 0;
char serial_wb[SERIAL_SIZE];

byte read_serial()
{
	register char b;

	b = Serial.read();
	if (b == -1)
		return 0;

	if (tty_mode == TTY_PASS) {
		if (b == '`') {
			set_bus_mode_controller();
			clear_read_buffer();
			return 0;
		}

		noInterrupts();
		//if (serial_read_head == serial_read_tail) {
		//	serial_read_head = 0;
		//	serial_read_tail = 0;	
		//}

		serial_rb[serial_read_tail++] = b;
		if (serial_read_tail >= SERIAL_SIZE)
			serial_read_tail = 0;
		interrupts();
		//Serial.write(serial_get_byte());
		return 0;
	}
	else if (tty_mode == TTY_COMMAND) {
		noInterrupts();
		serial_rb[serial_read_tail] = b;
		if (b == '\n' || b == '\r') {
			serial_rb[serial_read_tail] = '\0';
			serial_avail = 1;
		}
		if (serial_read_tail < SERIAL_SIZE)
			serial_read_tail++;
		interrupts();
		return serial_avail;
	}
}

inline byte serial_get_byte()
{
	register byte value;

	noInterrupts();
	if (serial_read_head == serial_read_tail) {
		//clear_read_buffer();
		value = -1;
	} else {
		value = serial_rb[serial_read_head++];
		if (serial_read_head >= SERIAL_SIZE)
			serial_read_head = 0;
	}
	interrupts();
	return value;
}

inline byte serial_add_byte(byte data)
{
	noInterrupts();
	serial_wb[serial_write_tail++] = data;
	interrupts();
}

void clear_read_buffer()
{
	noInterrupts();
	serial_avail = 0;
	serial_read_head = 0;
	serial_read_tail = 0;
	interrupts();
}

void flush_write_buffer()
{
	if (serial_write_tail > 0) {
		//take_bus();
		noInterrupts();
		for (byte i = 0; i < serial_write_tail; i++) {
			Serial.write(serial_wb[i]);
		}
		serial_write_head = 0;
		serial_write_tail = 0;
		interrupts();
		//if (is_running)
		//	release_bus();
	}
}



word send_size = 768;
byte send_mem[] = {

};

#define ROM_SIZE	0x1200
#define MEM_SIZE	0x1800
word mem_size = MEM_SIZE;
byte mem[MEM_SIZE] = {
	#include "/media/work/computie/software/output.txt"
};


inline void write_data(long addr, byte data)
{
	MEM_LDATA_DDR = 0xFF;
	MEM_LDATA_PORT = data;
	MEM_XADDR_PORT = (addr >> 16);
	MEM_HADDR_PORT = (addr >> 8) & 0xFF;
	MEM_LADDR_PORT = (0x00FF & addr);
	digitalWrite(MEM_WE, 0);
	digitalWrite(MEM_OE, 1);
	digitalWrite(MEM_CE, 0);
	delayMicroseconds(1);
	digitalWrite(MEM_WE, 1);
	digitalWrite(MEM_OE, 1);
	digitalWrite(MEM_CE, 1);
	INLINE_NOP;
	INLINE_NOP;
}

inline byte read_data(long addr)
{
	byte value = 0;

	MEM_LDATA_DDR = 0x00;
	MEM_XADDR_PORT = (addr >> 16);
	MEM_HADDR_PORT = (addr >> 8) & 0xFF;
	MEM_LADDR_PORT = 0x00FF & addr;
	digitalWrite(MEM_WE, 1);
	digitalWrite(MEM_OE, 0);
	digitalWrite(MEM_CE, 0);
	delayMicroseconds(1);
	value = MEM_LDATA_PIN;
	digitalWrite(MEM_WE, 1);
	digitalWrite(MEM_OE, 1);
	digitalWrite(MEM_CE, 1);
	INLINE_NOP;
	INLINE_NOP;

	return value;
}

void run_read_test()
{
	int lvalue = 0;
	int errors = 0;

	set_bus_mode_controller();
	Serial.print("Running Read Test\n");

	MEM_LDATA_PORT = 0x00;
	MEM_LDATA_DDR = 0x00;

	MEM_XADDR_PORT = 0;
	MEM_HADDR_PORT = 0;
	MEM_LADDR_PORT = 0;
	digitalWrite(MEM_WE, 1);
	for (long i = 0; i < MEM_TEST_SIZE; i++) {
		lvalue = read_data(i + MEM_TEST_START);

		if (lvalue != (i & 0x00FF))
			errors += 1;
		if (lvalue < 10) Serial.print("0");
		Serial.print(lvalue, HEX);
		Serial.print(" ");
		if (i % 64 == 63)
			Serial.print("\n");
	}

	Serial.print("\n");
	Serial.print("Errors: ");
	Serial.print(errors, DEC);
	Serial.print("\n");
}

void run_write_test()
{
	set_bus_mode_controller();
	Serial.print("Running Write Test\n");

	digitalWrite(MEM_WE, 1);
	digitalWrite(MEM_CE, 1);

	MEM_LDATA_PORT = 0x00;
	MEM_LDATA_DDR = 0xFF;

	MEM_HADDR_PORT = 0xFF;
	MEM_LADDR_PORT = 0;
	for (long i = 0; i < MEM_TEST_SIZE; i++) {
		MEM_LDATA_PORT = (i & 0x00FF);
		MEM_XADDR_PORT = (i >> 16) + MEM_TEST_HADDR;
		MEM_HADDR_PORT = (i >> 8) + MEM_TEST_HADDR;
		MEM_LADDR_PORT = 0x00FF & i;
		digitalWrite(MEM_WE, 0);
		digitalWrite(MEM_CE, 0);
		delayMicroseconds(1);
		digitalWrite(MEM_WE, 1);
		digitalWrite(MEM_CE, 1);
		MEM_HADDR_PORT = 0xFF;
		MEM_LADDR_PORT = 0;
		delayMicroseconds(1);
		if ((i % 64) == 63) {
			delayMicroseconds(10000);
		}
	}

	Serial.print("Complete\n");
}

void run_send_mem()
{
	word addr = (MEM_TEST_HADDR << 8);

	set_bus_mode_controller();

	MEM_LDATA_PORT = 0x00;
	MEM_LDATA_DDR = 0xFF;
	delayMicroseconds(1);
	for (word i = 0; i < mem_size; i += 2, addr += 1) {
		MEM_LDATA_PORT = mem[i + 1];
		MEM_HADDR_PORT = addr >> 8;
		MEM_LADDR_PORT = addr & 0xFF;
		digitalWrite(MEM_WE, 0);
		digitalWrite(MEM_CE, 0);
		delayMicroseconds(1);
		digitalWrite(MEM_WE, 1);
		digitalWrite(MEM_CE, 1);
		MEM_HADDR_PORT = 0xFF;
		MEM_LADDR_PORT = 0;
		delayMicroseconds(1);
		if ((addr % 64) == 63) {
			delayMicroseconds(10000);
		}
	}

	Serial.print("Sending complete\n");
}

void run_flash_erase()
{
	write_data(0x555, 0xAA);
	write_data(0x2AA, 0x55);
	write_data(0x555, 0x80);
	write_data(0x555, 0xAA);
	write_data(0x2AA, 0x55);
	write_data(0x00, 0x30);
	Serial.println("Supposedly erasing");
}


void run_flash_test()
{
	byte value;

	write_data(0x5555, 0xAA);
	write_data(0x2AAA, 0x55);
	write_data(0x5555, 0x90);
	value = read_data(0x00);
	Serial.print("MFR ID: ");
	Serial.print(value, HEX);
	Serial.print("\n");

	write_data(0x5555, 0xAA);
	write_data(0x2AAA, 0x55);
	write_data(0x5555, 0x90);
	value = read_data(0x01);
	Serial.print("DEV ID: ");
	Serial.print(value, HEX);
	Serial.print("\n");

	write_data(0x5555, 0xAA);
	write_data(0x2AAA, 0x55);
	write_data(0x5555, 0x90);
	value = read_data(0x03);
	Serial.print("CON ID: ");
	Serial.print(value, HEX);
	Serial.print("\n");
}

void run_flash_reset()
{
	write_data(0x0000, 0xF0);
	Serial.println("Supposedly reset");
}


void program_data(long addr, byte data)
{
	write_data(0x555, 0xAA);
	write_data(0x2AA, 0x55);
	write_data(0x555, 0xA0);
	write_data(addr, data);
}

void run_flash_program()
{
	char *str = "Hello\n";
	for (byte i = 0; i < 6; i++) {
		program_data(i, str[i]);
	}
	Serial.println("Programmed string");
}

void do_command(String line)
{
	if (line.equals("read")) {
		run_read_test();
	}
	else if (line.equals("write")) {
		run_write_test();
	}
	else if (line.equals("send")) {
		run_send_mem();
	}
	else if (line.equals("test")) {
		run_flash_test();
	}
	else if (line.equals("reset")) {
		run_flash_reset();
	}
	else if (line.equals("erase")) {
		run_flash_erase();
	}
	else if (line.equals("program")) {
		run_flash_program();
	}
}

void setup()
{
	Serial.begin(SERIAL_SPEED);

	set_bus_mode_controller();
	//set_bus_mode_device();

	pinMode(13, OUTPUT);
	digitalWrite(13, 0);

	Serial.print("> ");
}

void loop()
{
	if (read_serial() && tty_mode == TTY_COMMAND) {
		String line = String(serial_rb);
		clear_read_buffer();
		Serial.print(line);
		Serial.print("\n");
		do_command(line);
		if (tty_mode == TTY_COMMAND)
			Serial.print("\n> ");
	}
}

