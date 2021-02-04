
#include <stdlib.h>
#include <netinet/in.h>

char *inet_ntoa(struct in_addr in)
{
	static char ipa[18];
	unsigned char *bytes = (unsigned char *) &in.s_addr;

	snprintf(ipa, 18, "%d.%d.%d.%d", (int) bytes[0], (int) bytes[1], (int) bytes[2], (int) bytes[3]);
	return ipa;
}

