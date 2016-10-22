#include "AppLayer.h"
#include "DataLinkLayer.h"
#include "Utilities.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int appLayer(char *SerialPort, enum Functionality func) {

  if (openSerialPort(SerialPort) == -1) {
    printf("Error opening serial port\n");
    exit(-1);
  }
  if (setTermiosStructure() == -1) {
    printf("Error seting new termios\n");
    exit(-1);
  }

  llopen(SerialPort, func);

  if (func == TRANSMITER) {
    sendData();
  } else {
    receiveData();
  }

  return 1;
}

int sendData() {

  char *fileName = malloc(sizeof(char) * 255);
  getFile(fileName);
  FILE *fp;
  fp = fopen(fileName, "rb");
  if (fp == NULL) {
    printf("Could not open file  test.c");
  }
  printf("opened file");

  // Determine file size
  int size;
  size = fileSize(fp);
  printf("%d\n", size);

  unsigned char fileSize[50];
  sprintf(fileSize, "%d", size);

  int packetSize = 5 + strlen(fileName) + strlen(fileSize);

  unsigned char controlPacket[packetSize];
  int controlPacketSize = sendControlPackage(START_CTRL_PACKET, fileName, size, controlPacket);

  llwrite(fd, controlPacket, controlPacketSize);
}
int receiveData() {

  char fileName[255];

  //read(fd, fileName, 255);
  unsigned char buffer[255];
  llread(fd, buffer);
  printf(" FILE NAME:%s\n", fileName);
}

// TODO: Send control package with file name anda file size
int sendControlPackage(int state, char *fileName, int size, unsigned char *controlPacket) {

  //TODO: refector repeated code
  unsigned char fileSize[50];
  //	sprintf(fileSize, "%X", size);
	memcpy(fileSize,&size, sizeof(size));
	printf("%X",fileSize[0]);
	printf("%X",fileSize[1]);

  int controlPacketSize = 0;

  controlPacket[0] = (unsigned char)state;
  controlPacket[1] = (unsigned char)0; // 0-tamanho do ficheiro
  controlPacket[2] = (unsigned char)strlen(fileSize);
  controlPacketSize = 3;
  // um char é sempre um byte?
  int i;
  for (i = 0; i < strlen(fileSize); i++) {
    controlPacket[i + 3] = fileSize[i];
  }
  controlPacketSize += strlen(fileSize);

  controlPacket[controlPacketSize] =
      (unsigned char)1; // 0-tamanho do ficheiro
	controlPacketSize++;
  controlPacket[controlPacketSize] = (unsigned char)strlen(fileName);
	controlPacketSize++;

  for (i = 0; i < strlen(fileName); i++) {
    controlPacket[controlPacketSize + i] = fileName[i];
  }
  controlPacketSize += strlen(fileName);


  return controlPacketSize;
}
int llopen(char *SerialPort, enum Functionality func) {

  switch (func) {
  case TRANSMITER:
    llopenTransmiter(SerialPort);
    break;
  case RECEIVER:
    llopenReceiver(SerialPort);
    break;
  }
}
