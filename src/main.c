#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "DataLinkLayer.h"


int main(int argc, char** argv)
{

	if (argc == 0 || argc>2) {
		printf("ERROR! Not specified if you're the sender (argument = 0) or the receiver (arguement = 1)\n");
		exit(1);
	}

	if((strcmp("0", argv[1])==0)){
		printf("----SENDER----\n");
		writenoncanonical("/dev/ttyS0");

	}else if((strcmp("1", argv[1])==0)){
		printf("----Receiver---- \n");
		noncanonical("/dev/ttyS0");
	}
	return 0;
}
