#ifndef UTILITIES_H
#define UTILITIES_H
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int getFile(char *filepath);

int fileSize(FILE *fd);
#endif
