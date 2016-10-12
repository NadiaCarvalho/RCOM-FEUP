#include "AppLayer.h"
#include "DataLinkLayer.h"

appLayer(Functionality functionality, char * SerialPort){


  if(openSerialPort(char * SerialPort)==-1){
    printf("Error opening serial port\n");
    exit(-1);
  }

  if(setTermiosStructure()==-1){
    prinft("Error seting new termios\n");
    exit(-1);
  };

  //TODO: acabar
  llopen(functionality, mode);
}
