/*Non-Canonical Input Processing*/

#include "DataLinkLayer.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int flag = 1, tries = 0, success = 0, fail = 0;

void atende() {
  tries++;
  if (!success) {
    if (tries <= 3) {
      printf("alarme # %d\n", tries);
      alarm(3);
      // send SET
      printf("SENDER: sending SET\n");
      write(fd, SET, 5);
    } else if (tries == 4) {
      printf("Timeout: UA not acknowledge");
      exit(1);
    }
  }
}

ReadingArrayState nextState(ReadingArrayState state) {
  switch (state) {
  case START:
    state = FLAG_STATE;
    break;
  case FLAG_STATE:
    state = A_STATE;
    break;
  case A_STATE:
    state = C_STATE;
    break;
  case C_STATE:
    state = BCC;
    break;
  case BCC:
    state = SUCCESS;
    break;
  }

  return state;
}

int openSerialPort(char *SerialPort) {

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(SerialPort, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(SerialPort);
    return -1;
  }
  return fd;
}

int closeSerialPort(char *SerialPort) {
  // set old settings
  if (tcsetattr(SerialPort, TCSANOW, &oldtio) < 0) {
    printf("ERROR in closeSerialPort(): could not set old termios\n");
    return -1;
  }

  close(SerialPort);

  return 0;
}

int setTermiosStructure() {
  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    return -1;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1;  /* blocking read until 5 chars received */

  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) pr�ximo(s) caracter(es)
  */

  if (tcflush(fd, TCIOFLUSH) < 0) {
    perror("tcflush");
    return -1;
  }

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    return -1;
  }

  printf("New termios structure set\n");

  return 1;
}

int readingArray(int fd, char compareTo[]) {
  ReadingArrayState state = START;
  int res;

  char buf[255];

  int i = 0;
  while (1) {
    res = read(fd, buf, 1);
    printf("0x%08X", buf[0]);
    if (buf[0] == compareTo[i]) {
      i++;
      state = nextState(state);
      if (state == SUCCESS) {
        success = 1;
        return 1;
      }
    } else {
      if (buf[0] == FLAG_STATE) {
        state = FLAG_STATE;
      } else
        state = START;
    }
  }
}

int llopenTransmiter(char *SerialPort) {
  char buf[255];
  int i, sum = 0, speed = 0;

  // send SET
  res = write(fd, SET, 5);
  printf("SENDER: sending SET: 0x%08X , 0x%08X , 0x%08X , 0x%08X , 0x%08X\n",
         SET[0], SET[1], SET[2], SET[3], SET[4]);
  (void)signal(SIGALRM, atende); // instala  rotina que atende interrupcao

  strcpy(buf, "");

  alarm(3);
  if (readingArray(fd, UA)) {
    alarm(0);
  }

  printf("\nComunication established.\n");
  return 0;
}

int llopenReceiver(char *SerialPort) {
  char buf[255];

  strcpy(buf, "");
  printf("\nRECEIVER: reading SET\n");
  readingArray(fd, SET);

  printf("\nRECEIVER: sending UA\n");
  write(fd, UA, 5);

  printf("Sent UA frame.");
  return 0;
}

int llwrite(int fd, unsigned char *buffer, int length) {
  unsigned char frame[255];
  int sequenceNumber = 0;

  frame[0] = FLAG;
  frame[1] = A;
  frame[2] = (unsigned char)sequenceNumber;
  frame[3] = 0; // bcc1

  int i;
  for (i = 0; i < length; i++) {
    frame[i + 4] = buffer[i];
	//printf("%d : %02X \n", i, frame[i + 4]);
  }

  frame[length + 4] = 0; // bcc2
  frame[length + 5] = FLAG;

	for (i = 0; i < length +6; i++) {
	printf("%d : %02X \n", i, frame[i]);
  	}

  write(fd, frame, length + 6);

}

int llread(int fd, unsigned char *buffer) {

  unsigned char frame[255];
  unsigned char filename[255];
  int fileS;
  int over = 0;
  unsigned char fileName[255];
  FileInfo file;
  printf("\nVou começar a ler\n");

  int frameSize = readingFrame(fd, frame);

  // Processing frame
  if(frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_0 || frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_1){
    processingDataFrame(frame, &file);
  }

  printf("Terminei de ler\n");
}

int readingFrame(int fd, unsigned char* frame){
  unsigned char oneByte;
  int over = 0;
  int i = 0;

  // reading the first flag
  read(fd, &oneByte, 1);
  frame[i] = oneByte;
  i++;

  while(!over){
    read(fd, &oneByte, 1);
    frame[i] = oneByte;

    // testing if is the last character
    if(oneByte == FLAG){
      over = 1;
    }
    i++;
  }

  return i; // returning the size of the frame
}

int processingDataFrame(unsigned char *frame, FileInfo* file){
  int frameIndex = 4; // Where the packet starts
  int i;
  int numberOfBytes;

  // Testing to see if is a control packet
  if(frame[frameIndex] == 2 || frame[frameIndex] == 3){
    frameIndex+= 2; // TODO : Estou a ignorar o T

    numberOfBytes = frame[frameIndex];
    frameIndex++; // taking frame index to de begginning of the data
    memcpy(&((*file).size), frame+frameIndex, numberOfBytes);

    frameIndex += numberOfBytes + 1; // TODO : Estou a ignorar o T

    numberOfBytes = frame[frameIndex];
    frameIndex++;
    memcpy(&((*file).filename), frame+frameIndex, numberOfBytes);

    frameIndex += numberOfBytes;

    // TODO : processar o bcc2
  }
}
