#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "DataLinkLayer.h"

int main(int argc, char **argv) {

  enum Functionality func = TRANSMITER;

  if (argc == 0 || argc > 3) {
    printf("ERROR! Not specified if you're the sender (argument = 0) or the "
           "receiver (arguement = 1)\n");
    exit(1);
  }

  char serialPort[255] = "/dev/ttyS";
  strcat(serialPort, argv[2]);

  if (((strcmp("/dev/ttyS0", serialPort) != 0) &&
       (strcmp("/dev/ttyS1", serialPort) != 0))) {
    printf("Usage:\tnserial serialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  if ((strcmp("0", argv[1]) == 0)) {
    printf("\n----SENDER----\n\n");
    func = TRANSMITER;
    appLayer(serialPort, func);
  } else if ((strcmp("1", argv[1]) == 0)) {
    printf("\n----Receiver---- \n\n");
    func = RECEIVER;
    appLayer(serialPort, func);
  }
  return 0;
}
