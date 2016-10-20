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
  size= fileSize(fp);
  printf("%d\n",size );

  sendControlPackage(START, fileName,size);
}
int receiveData() {

  char fileName[255];

  read(fd, fileName, 255);

  printf(" FILE NAME:%s\n",fileName);
}

//TODO: Send control package with file name anda file size
int sendControlPackage(int state, char * fileName, int size){

    char fileSize[50];
    sprintf(fileSize,"%d",size);

    int packetSize = 5 + strlen(fileName) + strlen(fileSize);

    char controlPacket[packetSize];

    controlPacket[0]=state + '0'; // 48 is the ascii code for '0'
    controlPacket[1]=0 + '0'; //0-tamanho do ficheiro
    controlPacket[2]=strlen(fileSize) + '0';
    //um char é sempre um byte?
    int i;
    for(i=0; i<strlen(fileSize); i++){
      controlPacket[i+3]=fileSize[i];
    }
    printf("%d\n", strlen(controlPacket));



}


int llwrite(int fd, char *buffer, int length) {}

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
