
typedef unsigned char byte;

char *out = 0x00FE;
char *led = 0x00FF;

char *msg = "Welcome to the thing!\n";

void serial_print(char *str)
{
	byte i = 0;

	while (str[i] != '\0')
		*out = str[i++];
}


int main()
{
	*led = 0x01;

	serial_print(msg);

	return 0;
}

