#ifndef DATALINKLAYER_H
#define DATALINKLAYER_H

#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03

struct termios oldtio,newtio;


int fd, c, res;

enum Functionality { TRANSMITER, RECEIVER };

typedef enum {
	START,FLAG, A, C, BCC, SUCESS
} ReadingArrayState;




//SET = F-A-C-BCC-F
static char SET[5]={FLAG,A,C_SET,A^C_SET,FLAG};

static char UA[5]={FLAG,A,C_SET,A^C_SET,FLAG};

void atender();

int writenoncanonical(char* SerialPort);

int noncanonical(char* SerialPort);

int openSerialPort(char* SerialPort);

int setTermiosStructure();

int llopenTransmiter(char* SerialPort);

int llopenReceiver(char* SerialPort);


#endif
