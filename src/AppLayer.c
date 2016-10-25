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

  int seqNumber = 0;
  int seqAnswer;
  FileInfo file;
  getFile(file.filename);
  FILE *fp;
  fp = fopen(file.filename, "rb");
  if (fp == NULL) {
    printf("Could not open file  test.c");
    return -1;
  }
  printf("opened file %s\n", file.filename);

  // Determine file size
  file.size = fileSize(fp);
  printf("%d\n", file.size);

  unsigned char fileSize[50];
  memcpy(fileSize, &file.size, sizeof(file.size));

  int packetSize = 5 + strlen(file.filename) + strlen(fileSize);

  unsigned char controlPacket[packetSize];
  int controlPacketSize =
      sendControlPackage(START_CTRL_PACKET, file, controlPacket);

  llwrite(fd, controlPacket, controlPacketSize);

  int dataPacketSize;
  unsigned char dataPacket[DATA_SIZE + 4];
  int ret;

  while (ret != 0) {
    ret = sendDataPackage(dataPacket, fp, 0, &dataPacketSize);
    if (ret != 0) {
      //alarm(3);
      llwrite(fd, dataPacket, dataPacketSize);
    }
    seqNumber=checkAnswer(fd, seqNumber, dataPacket, dataPacketSize);
    //alarm(0);
  }

  controlPacketSize = sendControlPackage(END_CTRL_PACKET, file, controlPacket);

  llwrite(fd, controlPacket, controlPacketSize);
}

int checkAnswer(int fd, int seqNumber, unsigned char dataPacket[DATA_SIZE +4], int dataPacketSize){
  int seqTemp = receiveAnswer(fd, seqNumber);
  if (seqTemp == 1) {
    return 1;
  } else if (seqTemp == 0) {
    return 0;
  } else if (seqTemp == RETURN_REJ) {
      llwrite(fd, dataPacket, dataPacketSize);
      checkAnswer(fd, seqNumber, dataPacket, dataPacketSize);
  }
}

int receiveAnswer(int fd, int seqNumber) {
  int over = 0;
  unsigned char supervisionFrame[5];
  if (seqNumber == 0) {
    if (readingArray(fd, RR1, seqNumber) == 1)
      return 1;
  } else if (seqNumber == 1) {
    if (readingArray(fd, RR0, seqNumber) == 1)
      return 0;
  } else if (readingArray(fd, RR1, seqNumber) == RETURN_REJ)
    return RETURN_REJ;
}

int receiveData() {

  char fileName[255];

  // read(fd, fileName, 255);
  unsigned char buffer[255];
  llread(fd, buffer);
}

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket) {

  // TODO: refracting repeated code
  unsigned char fileSize[50];

  memcpy(fileSize, &file.size, sizeof(file.size));

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

  controlPacket[controlPacketSize] = (unsigned char)1; // 0-tamanho do ficheiro
  controlPacketSize++;
  controlPacket[controlPacketSize] = (unsigned char)strlen(file.filename);
  controlPacketSize++;

  for (i = 0; i < strlen(file.filename); i++) {
    controlPacket[controlPacketSize + i] = file.filename[i];
  }
  controlPacketSize += strlen(file.filename);

  return controlPacketSize;
}

int sendDataPackage(unsigned char *dataPacket, FILE *fp, int sequenceNumber,
                    int *length) {

  unsigned char buffer[DATA_SIZE];
  int ret;
  ret = fread(buffer, sizeof(char), DATA_SIZE, fp);
  if (ret == 0) {
    return 0;
  } else if (ret < 0) {
    return -1;
  }

  *length = ret + 4; // size of the data and the 4 initial bytes

  // K=256 * L1 + L2

  dataPacket[0] = DATA_CTRL_PACKET;
  dataPacket[1] = sequenceNumber;
  // L1
  dataPacket[2] = 0;
  // L2
  dataPacket[3] = ret;

  int j;
  for (j = 0; j < DATA_SIZE; j++) {
    dataPacket[4 + j] = buffer[j];
  }

  return 1;
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
