
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "prototype.h"

#define ECHO_BUF_SIZE	256

extern char ***environ;

int MAIN(command_echo)(int argc, char **argv, char **envp)
{
	// TODO this would normally be in _start
	//*environ = envp;

	int k = 0;
	char tmp;
	char *value;
	int open_quote = 0;
	char buffer[ECHO_BUF_SIZE];

	for (int i = 1; i < argc; i++) {
		for (int j = 0; argv[i][j]; j++) {
			if (argv[i][j] == '\"') {
				open_quote = !open_quote;
			}
			else if (argv[i][j] == '$') {
				int l;
				for (l = ++j; argv[i][l] && isalnum(argv[i][l]); l++) { }
				tmp = argv[i][l];
				argv[i][l] = '\0';
				value = getenv(&argv[i][j]);
				if (value) {
					strcpy(&buffer[k], value);
					k += strlen(&buffer[k]);
				}
				argv[i][l] = tmp;
				j = l - 1;
			}
			else
				buffer[k++] = argv[i][j];
		}
		buffer[k++] = ' ';
	}
	buffer[k] = '\0';

	puts(buffer);
	return 0;
}
