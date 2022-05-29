
#define SERIAL_SPEED	115200

#define MEM_ADDR	0x000000
#define MEM_SIZE	4096

#define FLASH_ADDR	0x000000

#define M68_XADDR_PORT	PORTF
#define M68_XADDR_PIN	PINF
#define M68_XADDR_DDR	DDRF

#define M68_HADDR_PORT	PORTC
#define M68_HADDR_PIN	PINC
#define M68_HADDR_DDR	DDRC

#define M68_LADDR_PORT	PORTL
#define M68_LADDR_PIN	PINL
#define M68_LADDR_DDR	DDRL

#define M68_LDATA_PORT	PORTA
#define M68_LDATA_PIN	PINA
#define M68_LDATA_DDR	DDRA

#define M68_HDATA_PORT	PORTK
#define M68_HDATA_PIN	PINK
#define M68_HDATA_DDR	DDRK
#define M68_A0		9

#define M68_DTACK	38 // PD7
#define M68_BG		39 // PG2
#define M68_BGACK	40 // PG1
#define M68_BR		41 // PG0

#define M68_DS		8
#define M68_AS		50 // PB3
#define M68_SIZ0	51 // PB2
#define M68_SIZ1	52 // PB1
#define M68_RW		53 // PB0

#define M68_ROMSEL	A7 // PF7

#define M68_RESET	10 // PB4
#define M68_BERR	11 // PB5

#define M68_IS_AS()	(!(PINB & 0x08))
#define M68_IS_WRITE()	(!(PINB & 0x01))
#define M68_IS_READ()	(PINB & 0x01)

#define M68_INIT_DTACK()	{ digitalWrite(M68_DTACK, 0); pinMode(M68_DTACK, INPUT); }
#define M68_ASSERT_DTACK()	(DDRD |= 0x80)
#define M68_UNASSERT_DTACK()	(DDRD &= ~(0x80))

#define M68_INIT_RESET()	{ digitalWrite(M68_RESET, 0); pinMode(M68_RESET, INPUT); }
#define M68_ASSERT_RESET()	(pinMode(M68_RESET, OUTPUT))
#define M68_UNASSERT_RESET()	(pinMode(M68_RESET, INPUT))



#undef INLINE_NOP
#define INLINE_NOP		__asm__("nop\n\t");

#define BUS_DEVICE	0
#define BUS_CONTROLLER	1

byte bus_request = 0;
byte bus_mode = BUS_DEVICE;

#define TTY_COMMAND	0
#define TTY_PASS	1

byte tty_mode = TTY_COMMAND;

void cpu_stop();

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

void take_bus()
{
	digitalWrite(M68_BR, 0);
	while (digitalRead(M68_BG) && !digitalRead(M68_AS)) { }
	digitalWrite(M68_BGACK, 0);
}

void release_bus()
{
	digitalWrite(M68_BR, 1);
	digitalWrite(M68_BGACK, 1);
}

void set_bus_mode_controller()
{
	//pciDisable(M68_AS);

	take_bus();

	// Controls
	pinMode(M68_AS, OUTPUT);
	pinMode(M68_DS, OUTPUT);
	pinMode(M68_SIZ0, OUTPUT);
	pinMode(M68_SIZ1, OUTPUT);
	pinMode(M68_RW, OUTPUT);
	pinMode(M68_DTACK, INPUT);
	digitalWrite(M68_DS, 1);
	digitalWrite(M68_AS, 1);
	digitalWrite(M68_RW, 1);
	digitalWrite(M68_SIZ0, 0);
	digitalWrite(M68_SIZ1, 0);

	// Address Bus
	M68_XADDR_PORT = 0x00;	// A17 - A23
	M68_XADDR_DDR = 0x7F;
	M68_HADDR_PORT = 0x00;	// A9 - A16
	M68_HADDR_DDR = 0xFF;
	M68_LADDR_PORT = 0x00;	// A1 - A8
	M68_LADDR_DDR = 0xFF;
	pinMode(M68_A0, OUTPUT);

	// Data Bus
	M68_LDATA_PORT = 0x00;	// D0 - D7
	M68_LDATA_DDR = 0x00;
	M68_HDATA_PORT = 0x00;	// D8 - D15
	M68_HDATA_DDR = 0x00;

	tty_mode = TTY_COMMAND;
	bus_mode = BUS_CONTROLLER;
}

void set_bus_mode_device()
{
	// Controls
	pinMode(M68_AS, INPUT);
	pinMode(M68_DS, INPUT);
	pinMode(M68_SIZ0, INPUT);
	pinMode(M68_SIZ1, INPUT);
	pinMode(M68_RW, INPUT);

	M68_INIT_DTACK();

	//pinMode(M68_RESET, OUTPUT);
	//digitalWrite(M68_RESET, 1);

	// Address Bus
	M68_XADDR_PORT = 0x00;	// A17 - A23
	M68_XADDR_DDR = 0x00;
	M68_HADDR_PORT = 0x00;	// A9 - A16
	M68_HADDR_DDR = 0x00;
	M68_LADDR_PORT = 0x00;	// A1 - A8
	M68_LADDR_DDR = 0x00;
	pinMode(M68_A0, INPUT);

	// Data Bus
	M68_LDATA_PORT = 0x00;	// D0 - D7
	M68_LDATA_DDR = 0x00;
	M68_HDATA_PORT = 0x00;	// D8 - D15
	M68_HDATA_DDR = 0x00;

	//pciSetup(M68_AS);
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
			cpu_stop();
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
		byte is_running = digitalRead(M68_BR);
		take_bus();
		noInterrupts();
		for (byte i = 0; i < serial_write_tail; i++) {
			Serial.write(serial_wb[i]);
		}
		serial_write_head = 0;
		serial_write_tail = 0;
		interrupts();
		if (is_running)
			release_bus();
	}
}

#define MAX_ARGS	10

int parse_serial_command(char **argv)
{
	int args = 1;

	argv[0] = serial_rb;
	for (int i = 0; i < serial_read_tail && serial_rb[i] != '\0'; i++) {
		if (serial_rb[i] == ' ') {
			serial_rb[i] = '\0';
			argv[args] = &serial_rb[i + 1];
			if (++args >= MAX_ARGS)
				break;
		}
	}
	argv[args] = NULL;

	return args;
}




word send_size = 768;
byte send_mem[] = {

};

#define ROM_SIZE	0x1800
#define ROM_MEM_SIZE	0x1800
word mem_size = ROM_MEM_SIZE;
byte mem[ROM_MEM_SIZE] = {
	#include "/media/work/computie/software/output.txt"
};



inline void write_data(long addr, word data)
{
	M68_HDATA_DDR = 0xFF;
	M68_LDATA_DDR = 0xFF;

	//M68_HDATA_PORT = (byte) (data >> 8);
	//M68_LDATA_PORT = (byte) data;
	M68_HDATA_PORT = (byte) data;

	M68_XADDR_PORT = (byte) (addr >> 17);
	M68_HADDR_PORT = (byte) (addr >> 9);
	M68_LADDR_PORT = (byte) (addr >> 1);
	digitalWrite(M68_A0, addr & 0x01);

	digitalWrite(M68_RW, 0);
	digitalWrite(M68_AS, 0);
	digitalWrite(M68_DS, 0);
	delayMicroseconds(1);
	digitalWrite(M68_DS, 1);
	digitalWrite(M68_AS, 1);
	digitalWrite(M68_RW, 1);
	M68_XADDR_PORT = (byte) 0xFF;
	INLINE_NOP;

	M68_HDATA_DDR = 0x00;
	M68_LDATA_DDR = 0x00;
}

inline byte read_data_byte(long addr)
{
	byte hvalue = 0;
	//byte lvalue = 0;

	M68_HDATA_DDR = 0x00;
	M68_LDATA_DDR = 0x00;

	M68_XADDR_PORT = (byte) (addr >> 17);
	M68_HADDR_PORT = (byte) (addr >> 9);
	M68_LADDR_PORT = (byte) (addr >> 1);
	digitalWrite(M68_A0, addr & 0x01);

	digitalWrite(M68_RW, 1);
	digitalWrite(M68_AS, 0);
	digitalWrite(M68_DS, 0);
	delayMicroseconds(1);
	hvalue = M68_HDATA_PIN;
	//lvalue = M68_LDATA_PIN;
	digitalWrite(M68_DS, 1);
	digitalWrite(M68_AS, 1);
	M68_XADDR_PORT = (byte) 0xFF;
	INLINE_NOP;

	return hvalue;
	//return ((word) hvalue << 8) | lvalue;
}

void run_erase_flash()
{
	Serial.println("Erasing flash");
	write_data(FLASH_ADDR + 0x555, 0xAA);
	write_data(FLASH_ADDR + 0x2AA, 0x55);
	write_data(FLASH_ADDR + 0x555, 0x80);
	write_data(FLASH_ADDR + 0x555, 0xAA);
	write_data(FLASH_ADDR + 0x2AA, 0x55);
	write_data(FLASH_ADDR + 0x00, 0x30);
}


void program_flash_data(long addr, byte data)
{
	write_data(FLASH_ADDR + 0x555, 0xAA);
	write_data(FLASH_ADDR + 0x2AA, 0x55);
	write_data(FLASH_ADDR + 0x555, 0xA0);
	write_data(addr, data);
}

inline void print_hex_byte(byte data)
{
	if (data < 16)
		Serial.write('0');
	Serial.print(data, HEX);
}

void run_send_mem()
{
	word i;
	long addr;

	set_bus_mode_controller();

	for (addr = FLASH_ADDR; addr < FLASH_ADDR + mem_size; addr++) {
		byte data = read_data_byte(addr);
		if (data != 0xFF) {
			Serial.print("Flash not erased at ");
			Serial.print(addr, HEX);
			Serial.print(" (");
			Serial.print(data, HEX);
			Serial.print(")\n");
			return;
		}
	}

	for (i = 0, addr = FLASH_ADDR; i < mem_size; i++, addr++) {
		byte data = mem[i];
		program_flash_data(addr, data);
		print_hex_byte(data);
		Serial.print("\n");
	}

	Serial.print("Sending complete\n");
}

void run_verify_mem()
{
	word i;
	long addr;

	for (i = 0, addr = FLASH_ADDR; i < mem_size; i++, addr++) {
		byte data = mem[i];
		byte actual_data = read_data_byte(addr);
		if (data != actual_data) {
			Serial.print("Expected ");
			Serial.print(data, HEX);
			Serial.print(" but found ");
			Serial.print(actual_data, HEX);
			Serial.print("\n");
		}
	}

	Serial.print("Verify complete\n");
}

void run_dump_mem(int argc, char **argv)
{
	word i;
	word size;
	long addr;

	if (argc > 1) {
		addr = strtol(argv[1], NULL, 16);
	} else {
		addr = FLASH_ADDR;
	}

	if (argc > 2) {
		size = strtol(argv[2], NULL, 16);
	} else {
		size = mem_size;
	}

	for (i = 0; i < size; i++, addr++) {
		byte data = read_data_byte(addr);
		print_hex_byte(data);
		Serial.print(" ");
		if ((addr % 64) == 63) {
			Serial.print("\n");
		}
	}
}

void run_write_test(int argc, char **argv)
{
	word i;
	word size;
	long addr;
	long start_addr;

	if (argc > 1) {
		start_addr = strtol(argv[1], NULL, 16);
	} else {
		start_addr = FLASH_ADDR;
	}

	if (argc > 2) {
		size = strtol(argv[2], NULL, 16);
	} else {
		size = mem_size;
	}

	for (i = 0, addr = start_addr; i < size; i++, addr += 4) {
		write_data(addr, (byte) i);
	}

	for (i = 0, addr = start_addr; i < size; i++, addr += 4) {
		byte data = read_data_byte(addr);
		print_hex_byte(data);
		Serial.print(" ");
		if ((addr % 64) == 60) {
			Serial.print("\n");
		}
	}
}


void cpu_start()
{
	set_bus_mode_device();
	tty_mode = TTY_PASS;
	release_bus();
	Serial.print("Running\n\n");
}

void cpu_stop()
{
	tty_mode = TTY_COMMAND;
	take_bus();
	set_bus_mode_controller();
	Serial.print("\nStopped\n");
}

void cpu_reset()
{
	M68_ASSERT_RESET();
	delay(10);
	M68_UNASSERT_RESET();
}


void do_command(int argc, char **argv)
{
	if (!strcmp(argv[0], "send")) {
		run_send_mem();
	}
	else if (!strcmp(argv[0], "verify")) {
		run_verify_mem();
	}
	else if (!strcmp(argv[0], "erase")) {
		run_erase_flash();
	}
	else if (!strcmp(argv[0], "dump")) {
		run_dump_mem(argc, argv);
	}
	else if (!strcmp(argv[0], "writetest")) {
		run_write_test(argc, argv);
	}
	else if (!strcmp(argv[0], "reset")) {
		set_bus_mode_device();
		cpu_reset();
	}
	else if (!strcmp(argv[0], "romsel")) {
		digitalWrite(M68_AS, 0);
		digitalWrite(M68_DS, 0);
		digitalWrite(M68_SIZ0, 1);
		digitalWrite(M68_SIZ1, 0);
		while (1) { }
	}
	else {
		Serial.println("Unknown command");
	}
}

void setup()
{
	Serial.begin(SERIAL_SPEED);

	M68_INIT_RESET();

	pinMode(M68_BR, OUTPUT);
	pinMode(M68_BGACK, OUTPUT);
	pinMode(M68_BG, INPUT);
	digitalWrite(M68_BR, 0);
	digitalWrite(M68_BGACK, 0);

	set_bus_mode_controller();
	//set_bus_mode_device();

	pinMode(13, OUTPUT);
	digitalWrite(13, 0);

	Serial.print("> ");

	// Run by default
	cpu_reset();
	cpu_stop();
}

void loop()
{
	if (read_serial() && tty_mode == TTY_COMMAND) {
		int args;
		char *argv[MAX_ARGS];

		Serial.print(serial_rb);
		Serial.print("\n");
		args = parse_serial_command(argv);
		do_command(args, argv);
		clear_read_buffer();
		if (tty_mode == TTY_COMMAND)
			Serial.print("\n> ");
	}

	//check_bus_cycle();
	//snoop_bus_cycle();
}

