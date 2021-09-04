
#define SERIAL_SPEED	115200

#define MEM_WR		41 // PG0
#define MEM_RD		40 // PG1
#define MEM_CS		39 // PG2


void init_bus()
{
	// Address Bus
	PORTL = 0x00;	// A8 - A15 (for bit banged)
	DDRL = 0xFF;
	PORTC = 0x00;	// A0 - A7 (for bit banged)
	DDRC = 0xFF;

	// Data Bus
	PORTA = 0x00;	// D0 - D7 (Also A0 - A7 for external mem interface)
	DDRA = 0x00;

	// Controls
	pinMode(MEM_CS, OUTPUT);
	pinMode(MEM_RD, OUTPUT);
	pinMode(MEM_WR, OUTPUT);
	digitalWrite(MEM_CS, 1);
	digitalWrite(MEM_RD, 1);
	digitalWrite(MEM_WR, 1);
	//PORTG = 0x07;
	//DDRG = 0x07;	// PG0 = /WR   PG1 = /RD    PG2 = /ALE
}

void write_data(int addr, byte data)
{
	PORTL = (addr >> 8);
	PORTC = (0x00FF & addr);
	PORTA = data;
	digitalWrite(MEM_WR, 0);
	digitalWrite(MEM_CS, 0);
	//delayMicroseconds(1);
	digitalWrite(MEM_WR, 1);
	digitalWrite(MEM_CS, 1);
}

byte read_data(int addr)
{
	byte value = 0;

	PORTL = (addr >> 8);
	PORTC = 0x00FF & addr;
	digitalWrite(MEM_RD, 0);
	digitalWrite(MEM_CS, 0);
	//delayMicroseconds(1);
	value = PINA;
	digitalWrite(MEM_RD, 1);
	digitalWrite(MEM_CS, 1);

	return value;
}

void run_write_test()
{
	Serial.print("Running Write Test\n");

	PORTA = 0x00;
	DDRA = 0xFF;
	for (int i = 0; i < 2048; i++) {
		PORTL = (i >> 8);
		PORTC = 0x00FF & i;
		PORTA = (i & 0x00FF);
		digitalWrite(MEM_WR, 0);
		digitalWrite(MEM_CS, 0);
		//delayMicroseconds(1);
		digitalWrite(MEM_WR, 1);
		digitalWrite(MEM_CS, 1);
	}

	Serial.print("Complete\n");
}

void run_read_test()
{
	int value = 0;
	int errors = 0;

	Serial.print("Running Read Test\n");

	PORTA = 0x00;
	DDRA = 0x00;
	for (int i = 0; i < 2050; i++) {
		PORTL = (i >> 8);
		PORTC = 0x00FF & i;
		digitalWrite(MEM_RD, 0);
		digitalWrite(MEM_CS, 0);
		//delayMicroseconds(1);
		value = PINA;
		digitalWrite(MEM_RD, 1);
		digitalWrite(MEM_CS, 1);

		if (value != (i & 0x00FF))
			errors += 1;
		Serial.print(value, HEX);
		Serial.print(" ");
		if (i % 64 == 63)
			Serial.print("\n");
	}

	Serial.print("\n");
	Serial.print("Errors: ");
	Serial.print(errors, DEC);
	Serial.print("\n");
}

inline byte convert_char(char ch) {
	if (ch <= 0x39)
		return ch - 0x30;
	else if (ch <= 0x46)
		return ch - 0x37;
	else
		return ch - 0x57;
}

void run_send()
{
	int addr = 0;
	String data = "";
	unsigned char value = 0;

	//Serial.setTimeout(60000);
	Serial.print("Waiting for data...\n");

	while (!data || data.length() <= 0) {
		data = Serial.readString();
	}

	Serial.print("You entered: ");
	Serial.print(data);
	Serial.print("\n");

	PORTA = 0x00;
	DDRA = 0xFF;
	for (int i = 0; i < data.length() - 1; i += 2) {
		value = (convert_char(data[i]) << 4) | convert_char(data[i + 1]);
		write_data(addr++, value);
		Serial.print("Wrote ");
		Serial.print(value, HEX);
		Serial.print("\n");
	}

	Serial.print("Sending complete\n");
}

#define INLINE_NOP		__asm__("nop\n\t");

void write_to_8155(int addr, byte data)
{
	PORTA = 0x00;
	DDRA = 0xFF;

	PORTA = 0x00FF & addr;	// address lower
	PORTL = (addr >> 8);	// address upper
	digitalWrite(MEM_CS, 1);
	//delayMicroseconds(1);
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	digitalWrite(MEM_CS, 0);
	PORTA = data;
	digitalWrite(MEM_WR, 0);
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	//delayMicroseconds(1);
	digitalWrite(MEM_WR, 1);
}

void run_8155_test()
{
	byte value = 0;

	Serial.print("8155 Test\n");

	write_to_8155(0x1100, 0x02);
	delayMicroseconds(1);
	write_to_8155(0x1102, 0xFF);
	delayMicroseconds(1);

	PORTA = 0x00;
	DDRA = 0xFF;

	PORTA = 0x00;	// address lower
	PORTL = 0x11;	// address upper
	digitalWrite(MEM_CS, 1);
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	//delayMicroseconds(1);
	digitalWrite(MEM_CS, 0);
	INLINE_NOP;
	digitalWrite(MEM_RD, 0);
	INLINE_NOP;
	PORTA = 0x00;
	DDRA = 0x00;
	value = PINA;
	//delayMicroseconds(1);
	digitalWrite(MEM_RD, 1);
	Serial.print(value, HEX);
	Serial.print("\n");
}

void setup()
{
	Serial.begin(SERIAL_SPEED);

	init_bus();
}

void loop()
{
	String line = Serial.readString();
	if (line.equals("read\n")) {
		run_read_test();
	}
	else if (line.equals("write\n")) {
		run_write_test();
	}
	else if (line.equals("send\n")) {
		run_send();
	}
	else if (line.equals("8155\n")) {
		run_8155_test();
	}
}

