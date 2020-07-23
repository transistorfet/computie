
typedef unsigned char byte;
typedef unsigned int word;

__sfr __at (0x00F0) status;
__sfr __at (0x00F1) in;
__sfr __at (0x00F1) out;
__sfr __at (0x00FF) led;

char *msg = "Welcome to the thing!\n";

void serial_print(char *str)
{
	byte i = 0;

	while (str[i] != '\0')
		out = str[i++];
}

void serial_read_loop()
{
	word i;
	byte ch;

	while (1) {
		if (status & 0x80) {
			ch = in;
			while (!(status & 0x40)) { }
			out = ch;
		}
		//for (i = 0; i < 10; i++) { }
	}
}


int main()
{
	word i;

	led = 0x01;

	serial_print(msg);
	//serial_read_loop();

	return 0;
}

