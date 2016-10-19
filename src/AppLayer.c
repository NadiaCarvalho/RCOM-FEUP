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

  sendControlPackage(fileName,size);
}
int receiveData() {

  char fileName[255];

  read(fd, fileName, 255);

  printf(" FILE NAME:%s\n",fileName);
}

//TODO: Send control package with file name anda file size
int sendControlPackage(char * fileName, int size){

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
