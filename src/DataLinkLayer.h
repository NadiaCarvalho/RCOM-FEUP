#ifndef DATALINKLAYER_H
#define DATALINKLAYER_H

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "AppLayer.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define ESC 0x7D
#define FLAG_HIDE_BYTE 0x5E
#define ESC_HIDE_BYTE 0x5D
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0B
#define FILE_SIZE 0
#define FILE_NAME 1
#define NUMBER_OF_SEQUENCE_0 0x00
#define NUMBER_OF_SEQUENCE_1 0x40
#define FIELD_CONTROL 2
#define C_RR0 0x05
#define C_RR1 0x85
#define C_REJ 0x01

struct termios oldtio, newtio;

int fd, c, res;

// WARNING: use diferente name for the state from global variables...
typedef enum {
  START,
  FLAG_STATE,
  A_STATE,
  C_STATE,
  BCC,
  SUCCESS
} ReadingArrayState;


static char SET[5] = {FLAG, A, C_SET, A ^ C_SET, FLAG};

static char UA[5] = {FLAG, A, C_UA, A ^ C_UA, FLAG};

static char RR0[5] = {FLAG, A, C_RR0, A ^ C_RR0, FLAG};

static char RR1[5] = {FLAG, A, C_RR1, A ^ C_RR1, FLAG};

static char REJ[5] = {FLAG, A, C_REJ, A ^ C_REJ, FLAG};

static char DISC[5] = {FLAG, A, C_DISC, A ^ C_DISC, FLAG};

void atender();

void retry();

int writenoncanonical(char *SerialPort);

int noncanonical(char *SerialPort);

int openSerialPort(char *SerialPort);

int setTermiosStructure();

int llopenTransmiter(char *SerialPort);

int llopenReceiver(char *SerialPort);

int llwrite(int fd, unsigned char *buffer, int length);

int llread(int fd, unsigned char *frame);

int llclose(int fd, enum Functionality func);

int readingFrame(int fd, unsigned char *frame);

int processingDataFrame(unsigned char *frame);

int stuffingFrame(unsigned char *frame, int frameSize);

int shiftFrame(unsigned char *frame, int i, int frameSize, int shiftDirection);

int destuffingFrame(unsigned char *frame);

unsigned char getBCC2(unsigned char *frame, unsigned int length);

#endif
