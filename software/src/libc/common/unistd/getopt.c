
#include <stddef.h>
#include <string.h>
#include <unistd.h>

int optind = 1;
int opterr = 0;
int optopt;
char *optarg;

int getopt(int argc, char * const argv[], const char *optstring)
{
	static int nextchar = 1;
	const char *chopt;

	if (nextchar == 1) {
		if (optind >= argc || argv[optind][0] != '-' || argv[optind][0] == '\0')
			return EOF;
		else if (!strcmp(argv[optind], "--")) {
			optind++;
			return EOF;
		}
	}

	optopt = argv[optind][nextchar++];
	if (optopt == ':' || (chopt = strchr(optstring, optopt)) == NULL) {
		// TODO print error
		optopt = '?';
	}
	else if (chopt[1] == ':') {
		if (argv[optind][nextchar] != '\0' || ++optind >= argc) {
			// TODO print error
			optopt = '?';
		}
		else {
			optarg = argv[optind++];
			nextchar = 1;
		}
	}

	if (argv[optind][nextchar] == '\0') {
		optind += 1;
		nextchar = 1;
	}

	return optopt;
}

