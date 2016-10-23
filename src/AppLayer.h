#ifndef APPLAYER_H
#define APPLAYER_H

#define START_CTRL_PACKET 2
#define END_CTRL_PACKET 3
#define DATA_SIZE 100
#define MAX_SIZE 255

enum Functionality { TRANSMITER, RECEIVER };

typedef struct {
  unsigned int size;
  unsigned char filename[MAX_SIZE];
} FileInfo;

int appLayer(char * SerialPort, enum Functionality functionality);

int llopen(char * SerialPort, enum Functionality functionality);

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket);
#endif
