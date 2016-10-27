#ifndef APPLAYER_H
#define APPLAYER_H

#define DATA_CTRL_PACKET 1
#define START_CTRL_PACKET 2
#define END_CTRL_PACKET 3
#define DATA_SIZE 100
#define MAX_SIZE 255

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

enum Functionality { TRANSMITER, RECEIVER };

typedef struct {
  unsigned int size;
  char filename[MAX_SIZE];
} FileInfo;
int sendData();

int receiveData();

int appLayer(char *SerialPort, enum Functionality functionality);

int llopen(char *SerialPort, enum Functionality functionality);

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket);

int sendDataPackage(unsigned char *dataPacket, FILE *fp, int sequenceNumber, int *length);

#endif
