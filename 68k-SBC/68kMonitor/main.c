
typedef unsigned char byte;
typedef unsigned int word;

char *led = (char *) 0x02ff;
char *out = (char *) 0x02f1;

char *msg = "Welcome to the thing!\n";

void serial_print(char *str)
{
	byte i = 0;

	while (str[i] != '\0')
		*out = str[i++];
}

/*
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
*/

int main()
{
	*led = 0x01;

	serial_print(msg);
	//serial_read_loop();

	return 0;
}

