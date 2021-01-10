
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototype.h"

int MAIN(command_date)(int argc, char **argv, char **envp)
{
	time_t t;
	char buffer[100];
	struct tm *current_time;

	if (argc >= 2) {
		t = strtol(argv[1], NULL, 10);
		stime(&t);
	}

	t = time(NULL);
	//t = 1604635268;
	current_time = gmtime(&t);

	//printf("%d\n", t);
	//printf("%d/%02d/%02d %02d:%02d:%02d\n", current_time->tm_year, current_time->tm_mon + 1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
	strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", current_time);
	puts(buffer);

	return 0;
}
