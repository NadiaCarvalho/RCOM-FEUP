#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include "DataLinkLayer.h"
#include "AppLayer.h"
#include "Utilities.h"

unsigned char previousDataCounter = 0;
int dataSize = 100; //default value

int appLayer(char *SerialPort, enum Functionality func) {

  if (openSerialPort(SerialPort) == -1) {
    printf("Error opening serial port\n");
    exit(-1);
  }
  if (setTermiosStructure() == -1) {
    printf("Error seting new termios\n");
    exit(-1);
  }

  if(func == TRANSMITER){
    askNumberOfTries();
    dataSize = askMaxFrameSize();
  }

  askTimeOfTimeout();

  llopen(SerialPort, func);

  if (func == TRANSMITER) {
    sendData();
  } else {
    receiveData();
  }

  llclose(fd, func);

  return 1;
}

int sendData() {

  char sequenceNumber = NUMBER_OF_SEQUENCE_0;
  int dataCounter = 1;
  FileInfo file;
  getFile(file.filename);
  FILE *fp;
  fp = fopen(file.filename, "rb");
  if (fp == NULL) {
    printf("Could not open file  test.c");
    return -1;
  }
  printf("opened file %s\n", file.filename);
  (void)signal(SIGALRM, retry);

  // Determine file size
  file.size = fileSize(fp);
  printf("File size : %d\n", file.size);

  char fileSize[50];
  memcpy(fileSize, &file.size, sizeof(file.size));

  int packetSize = 5 + strlen(file.filename) + strlen(fileSize);

  unsigned char controlPacket[packetSize];

  int controlPacketSize =
      sendControlPackage(START_CTRL_PACKET, file, controlPacket);

  controlPacket[controlPacketSize] = sequenceNumber;
  controlPacketSize++;

  llwrite(fd, controlPacket, controlPacketSize);

  int dataPacketSize;
  unsigned char dataPacket[dataSize + 4];
  int ret = 1;
  int numberOfRejs = 0;
  int llwriteRet;

  while (ret != 0) {
    ret = sendDataPackage(dataPacket, fp, dataCounter, &dataPacketSize);

    if (dataCounter < 255)
      dataCounter++;
    else
      dataCounter = 1;
    if (ret != 0) {
      dataPacket[dataPacketSize] = sequenceNumber;
      dataPacketSize++;
      llwrite(fd, dataPacket, dataPacketSize);
      if (sequenceNumber == NUMBER_OF_SEQUENCE_0)
        sequenceNumber = NUMBER_OF_SEQUENCE_1;
      else
        sequenceNumber = NUMBER_OF_SEQUENCE_0;
    }
  }

  controlPacketSize = sendControlPackage(END_CTRL_PACKET, file, controlPacket);
  controlPacket[controlPacketSize] = sequenceNumber;
  controlPacketSize++;

  llwriteRet = llwrite(fd, controlPacket, controlPacketSize);
  numberOfRejs += llwriteRet;

  printf("\nFile sent\n\n");

  printf("Number of rejs reiceived : %d\n", numberOfRejs);

  return 1;
}

int receiveData() {
  unsigned char frame[255];
  int over = 0;
  FileInfo file;
  file.size = 0;
  int ret;
  int fp;
  int bytesRead = 0;
  int percentage = 0;
  int packagesLost = 0;
  int percentageWrite = 0;
  int numberOfRejs = 0;
  int frameLength;

  printf("\nStart reading\n");

  while (!over) {

    frameLength = llread(fd, frame);

    if (frameLength == -1) {
      packagesLost++;
      ret = -1;
    }
    else {
       ret = processingDataPacket(frame, frameLength, &file, fp);

       if (ret == START_CTRL_PACKET) {
         fp = open(file.filename, O_CREAT | O_WRONLY);
         if (fp == -1) {
           printf("Could not open file  test.c");
           return -1;
         }
       }

       if (ret == DATA_CTRL_PACKET) {
         bytesRead += frameLength - 10;
         percentage = (bytesRead * 100) / file.size;
         if((percentage % 10) == 0 && percentageWrite != (int)percentage){
           printf("%d%%\n", percentage);
           percentageWrite = (int)percentage;
         }

       }

       if (ret == END_CTRL_PACKET) {
         over = 1;
       }
    }

    if (ret == -1) {
      write(fd, REJ, 5);
      numberOfRejs++;
    } else {
      if (frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_0) {
        write(fd, RR1, 5);
      } else
        write(fd, RR0, 5);
    }
  }

  printf("\nFile read\n");
  printf("\nPackages lost : %d\n", packagesLost);
  printf("Total bytes read : %d\n", bytesRead);
  printf("File size : %d\n", file.size);
  printf("Number of rejs sent : %d\n", numberOfRejs);

  return 1;
}

int processingDataPacket(unsigned char *packet, int length, FileInfo *file, int fp){

  int index = 4;
  int numberOfBytes;
  int ret;
  int dataCounterCheck = 0;

  // Testing to see if is a control packet
  if (packet[index] == START_CTRL_PACKET ||
      packet[index] == END_CTRL_PACKET) {

    ret = packet[index];
    index += 2;

    numberOfBytes = packet[index];
    index++; // taking packet index to de begginning of the data
    memcpy(&((*file).size), packet + index, numberOfBytes);

    index += numberOfBytes + 1;

    numberOfBytes = packet[index];
    index++;
    memcpy(&((*file).filename), packet + index, numberOfBytes);

    index += numberOfBytes;

    if (ret == START_CTRL_PACKET) {
      printf("File name : %s\n", file->filename);
      printf("File size : %d\n\n", file->size);
    }
  }
  else if (packet[index] == DATA_CTRL_PACKET) {

    ret = packet[index];
    index++;
    int counterIndex = index;
    index++;

    unsigned int l2 = packet[index];
    index++;
    unsigned int l1 = packet[index];
    index++;
    unsigned int k = 256 * l2 + l1;
    if (packet[8 + k] != getBCC2(packet + 4, k + 4)) {
      printf("BCC received: %X\n", packet[8 + k]);
      printf("BCC expected: %X\n", getBCC2(packet + 4, k + 4));
      return -1;
    }

    if (k != (length - 10)) {
      printf("ERRO\n");
      return -1;
    }

    if (previousDataCounter == 0) {
      previousDataCounter = packet[counterIndex];
    } else {
      if (previousDataCounter == packet[counterIndex]) {
        dataCounterCheck = 1;
        printf("Repeated packet\n");
      } else {
        previousDataCounter = packet[counterIndex];
        dataCounterCheck = 0;
      }
    }

    int i;
    for (i = 0; i < k; i++) {
      if (dataCounterCheck == 0)
        write(fp, &packet[index + i], 1);
    }
  }

  return ret;
}

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket) {

  // TODO: refracting repeated code
  char fileSize[50];

  memcpy(fileSize, &file.size, sizeof(file.size));

  int controlPacketSize = 0;

  controlPacket[0] = (unsigned char)state;
  controlPacket[1] = (unsigned char)0; // 0-tamanho do ficheiro
  controlPacket[2] = (unsigned char)strlen(fileSize);
  controlPacketSize = 3;
  // um char é sempre um byte?
  unsigned int i;
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

  unsigned char buffer[dataSize];
  int ret;
  ret = fread(buffer, sizeof(char), dataSize, fp);
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
  for (j = 0; j < dataSize; j++) {
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

  return 1;
}
