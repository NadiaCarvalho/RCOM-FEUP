#ifndef APPLAYER_H
#define APPLAYER_H

#define START_CTRL_PACKET 2
#define END_CTRL_PACKET 3

enum Functionality { TRANSMITER, RECEIVER };


int appLayer(char * SerialPort, enum Functionality functionality);

int llopen(char * SerialPort, enum Functionality functionality);

#endif
