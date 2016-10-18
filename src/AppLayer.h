#ifndef APPLAYER_H
#define APPLAYER_H

enum Functionality { TRANSMITER, RECEIVER };

int appLayer(char * SerialPort, enum Functionality functionality);

int llopen(char * SerialPort, enum Functionality functionality);

#endif
