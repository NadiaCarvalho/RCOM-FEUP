#ifndef DATALINKLAYER_H
#define DATALINKLAYER_H

#include "AppLayer.h"
#include <stdio.h>
#include <termios.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define ESC 0x7D
#define FLAG_HIDE_BYTE 0x5E
#define ESC_HIDE_BYTE 0x5D
#define A_SENDER_RECEIVER 0x03
#define A_RECEIVER_SENDER 0x01
#define C_SET 0x03
#define C_UA 0x07
#define C_DATA_SEQUENCE_0 0x00
#define C_DATA_SEQUENCE_1 0x40
#define FILE_SIZE 0
#define FILE_NAME 1
#define NUMBER_OF_SEQUENCE_0 0x00
#define NUMBER_OF_SEQUENCE_1 0x40
#define FIELD_CONTROL 2

struct termios oldtio, newtio;

int fd, c, res;

// WARNING: use diferente name for the state from global variables...
typedef enum {
  START,
  FLAG_STATE,
  A_STATE,
  C_STATE,
  BCC,
  SUCCESS,
  CONTROL_C,
  CONTROL_T1,
  CONTROL_L1,
  CONTROL_DATA1,
  CONTROL_T2,
  CONTROL_L2,
  CONTROL_DATA2,
  DATA_C,
  DATA_N,
  DATA_L2,
  DATA_L1,
  DATA_FIELD,
  BCC2,
  FINAL_FLAG
} ReadingArrayState;

// SET = F-A-C-BCC-F
static char SET[5] = {FLAG, A_SENDER_RECEIVER, C_SET, A_SENDER_RECEIVER ^ C_SET, FLAG};

static char UA[5] = {FLAG, A_SENDER_RECEIVER, C_UA, A_SENDER_RECEIVER ^ C_UA, FLAG};

void atender();

int writenoncanonical(char *SerialPort);

int noncanonical(char *SerialPort);

int openSerialPort(char *SerialPort);

int setTermiosStructure();

int llopenTransmiter(char *SerialPort);

int llopenReceiver(char *SerialPort);

int llwrite(int fd, unsigned char *buffer, int length);

int llread(int fd, unsigned char *buffer);

int readingFrame(int fd, unsigned char *frame);

int processingDataFrame(unsigned char *frame, FileInfo *file, int fp);

int stuffingFrame(unsigned char *frame, int frameSize);

int shiftFrame(unsigned char *frame, int i, int frameSize, int shiftDirection);

int destuffingFrame(unsigned char *frame);

#endif
