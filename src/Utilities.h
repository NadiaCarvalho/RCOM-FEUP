#ifndef UTILITIES_FILE
#define UTILITIES_FILE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int getFile(char *filepath);

int fileSize(FILE *fd);
#endif
