
#include <stdio.h>

FILE _stdin = { 0 };
FILE _stdout = { 1 };
FILE _stderr = { 2 };

FILE *stdin = &_stdin;
FILE *stdout = &_stdout;
FILE *stderr = &_stderr;

