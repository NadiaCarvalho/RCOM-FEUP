#ifndef DATALINKLAYER_H
#define DATALINKLAYER_H

#include <stdio.h>
#include <termios.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07
#define FILE_SIZE 0
#define FILE_NAME 1

struct termios oldtio, newtio;

int fd, c, res;

// WARNING: use diferente name for the state from global variables...
typedef enum { START, FLAG_STATE, A_STATE, C_STATE, BCC, SUCCESS } ReadingArrayState;

// SET = F-A-C-BCC-F
static char SET[5] = {FLAG, A, C_SET, A ^ C_SET, FLAG};

static char UA[5] = {FLAG, A, C_UA, A ^ C_UA, FLAG};

void atender();

int writenoncanonical(char *SerialPort);

int noncanonical(char *SerialPort);

int openSerialPort(char *SerialPort);

int setTermiosStructure();

int llopenTransmiter(char *SerialPort);

int llopenReceiver(char *SerialPort);

int llwrite(int fd, unsigned char * buffer, int length);

int llread(int fd, unsigned char *buffer);

int receiveControlPacket(int fd, unsigned char *filename);

#endif
