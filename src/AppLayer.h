#ifndef APPLAYER_FINAL
#define APPLAYER_FINAL

#define DATA_CTRL_PACKET 1
#define START_CTRL_PACKET 2
#define END_CTRL_PACKET 3
#define DATA_SIZE 100
#define MAX_SIZE 255

#include <stdio.h>
#include <stdlib.h>

enum Functionality { TRANSMITER, RECEIVER };

typedef struct {
  unsigned int size;
  unsigned char filename[MAX_SIZE];
} FileInfo;

int appLayer(char *SerialPort, enum Functionality functionality);

int llopen(char *SerialPort, enum Functionality functionality);

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket);

int sendData();

int checkAnswer(int fd, int seqNumber, unsigned char dataPacket[DATA_SIZE +4], int dataPacketSize);

int receiveAnswer(int fd, int seqNumber);

int receiveData();

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket);

int sendDataPackage(unsigned char *dataPacket, FILE *fp, int sequenceNumber,int *length);

#endif
