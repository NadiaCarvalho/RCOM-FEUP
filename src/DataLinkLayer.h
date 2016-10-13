#pragma once

#include <termios.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03

struct termios oldtio,newtio;

int flag=1, conta=1, success = 0, fail = 0;
int fd, c, res;


typedef enum {
  TRANSMITER = 0, RECEIVER = 1
} Functionality=TRANSMITER;


//SET = F-A-C-BCC-F
static char SET[5]={FLAG,A,C_SET,A^C_SET,FLAG};

static char UA[5]={FLAG,A,C_SET,A^C_SET,FLAG};

void atender();

int writenoncanonical(char* SerialPort);

int noncanonical(char* SerialPort);

int openSerialPort(char* SerialPort);
