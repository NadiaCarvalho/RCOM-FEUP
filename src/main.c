#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "DataLinkLayer.h"
#include "AppLayer.h"

int main(int argc, char** argv)
{

	enum Functionality functionality = TRANSMITER;

	if (argc == 0 || argc>3) {
		printf("ERROR! Not specified if you're the sender (argument = 0) or the receiver (arguement = 1)\n");
		exit(1);
	}

	char serialPort[255] = "/dev/ttyS";
	strcat(serialPort,argv[2]);


	if (((strcmp("/dev/ttyS0", SerialPort)!=0) &&
	(strcmp("/dev/ttyS1", SerialPort)!=0) )) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}

	if((strcmp("0", argv[1])==0)){
		printf("----SENDER----\n");
		functionality=TRANSMITER;
		appLayer(argv[2], functionality);
	}else if((strcmp("1", argv[1])==0)){
		printf("----Receiver---- \n");
		functionality=RECEIVER;
		appLayer(argv[2], functionality);
	}
	return 0;
}
