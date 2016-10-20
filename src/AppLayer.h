#ifndef APPLAYER_H
#define APPLAYER_H

#define START_CTRL_PACKET 2
#define END_CTRL_PACKET 3
#define DATA_SIZE 100

enum Functionality { TRANSMITER, RECEIVER };


int appLayer(char * SerialPort, enum Functionality functionality);

int llopen(char * SerialPort, enum Functionality functionality);

int sendControlPackage(int state, char *fileName, int size, unsigned char *controlPacket);
#endif
