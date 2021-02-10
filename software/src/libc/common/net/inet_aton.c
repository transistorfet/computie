
#include <stdlib.h>
#include <netinet/in.h>

int inet_aton(const char *str, struct in_addr *ptr)
{
	long octet;
	char *endptr;
	in_addr_t addr = 0;

	for (short i = 0; i < 4; i++) {
		addr <<= 8;
		octet = strtol(str, &endptr, 10);
		if (octet < 0 || octet > 255)
			return 0;
		addr |= (octet & 0xFF);

		if (!endptr || *endptr == '\0')
			break;
		if (*endptr != '.')
			return 0;
		str = endptr + 1;
	}

	ptr->s_addr = addr;
	return 1;
}

