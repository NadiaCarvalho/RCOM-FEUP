#include "AppLayer.h"
#include "DataLinkLayer.h"
#include "Utilities.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
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

  if(func == TRANSMITER){
    sendData();
  }else{
    receiveData();
  }


  return 1;
}

int sendData(){

    char file[255];
    file=getFile();
    printf("%s\n",file );;
    char buf[255];
    int fdFileToSend = open(file, O_RDONLY);
    printf("opend file" );

    //Determine file sizeo
    struct stat st;
    stat(file, &st);
    int fileSize=st.st_size;
    printf("file size:" );
    printf("%s\n", fileSize );

    //sendControlPackage(file, );

    while(read(fdFileToSend, buf, 8)){
      //  llwrite(fd, buf, 8)
    }


}
int receiveData(){

}

int llwrite(int fd, char * buffer, int length){

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
