/*Non-Canonical Input Processing*/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "DataLinkLayer.h"

int flag = 1, tries = 0, success = 0, fail = 0;
unsigned char frame[255];
char temp[5];
int frameSize=0;
unsigned char previousDataCounter = 0;

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

void retry(){
		alarm(3);
		//tcflush(fd,TCOFLUSH);
	  write(fd, frame, frameSize);
	  //read(fd,temp,5);
	  //alarm(0);
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
  case SUCCESS:
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

  char buf[255];

  int i = 0;
  while (1) {
    res = read(fd, buf, 1);
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

  // send SET
  res = write(fd, SET, 5);
  printf("SENDER: sending SET\n");
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

  int sequenceNumber = buffer[length-1];
	length--;
  frame[0] = FLAG;
  frame[1] = A;
  // TODO: campo de control != sequenceNumber
  frame[2] = sequenceNumber;
  frame[3] = frame[1]^frame[2];

  int i;
  for (i = 0; i < length; i++) {
    frame[i + 4] = buffer[i];
    // printf("%d : %02X \n", i, frame[i + 4]);
  }

  frame[length + 4] = getBCC2(buffer,length);
	printf("BCC: %X",frame[length+4]);
  frame[length + 5] = FLAG;

  for (i = 0; i < length + 6; i++) {
    printf("%d : %02X \n", i, frame[i]);
  }
 (void)signal(SIGALRM, retry);
  frameSize = stuffingFrame(frame, length + 6);
	do{
alarm(3);
  write(fd, frame, frameSize);
	read(fd,temp,5);
	alarm(0);
}while(temp[2]==C_REJ);

return 1;

}

int llread(int fd, unsigned char *buffer) {
int sizeAfterDestuffing=0;
  unsigned char frame[255];
  int over = 0;
  FileInfo file;
  int ret;
  int fp;
  
  printf("\nVou começar a ler\n");

  while (!over) {

    readingFrame(fd, frame);

   sizeAfterDestuffing = destuffingFrame(frame);

    // Processing frame
    if (frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_0 ||
        frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_1) {
      	ret = processingDataFrame(frame, &file, fp, sizeAfterDestuffing);
    }

	if(ret == START_CTRL_PACKET){	
	  	fp = open("teste.gif", O_CREAT | O_WRONLY);
  		if (fp == -1) {
    	printf("Could not open file  test.c");
    	return -1;
  		}
	}

    if (ret == END_CTRL_PACKET) {
      over = 1;
    }
	if(ret==-1){
		write(fd,REJ,5);
	}
	else {
		if(frame[FIELD_CONTROL] == NUMBER_OF_SEQUENCE_0){
		write(fd,RR1,5);
		}else
		write(fd,RR0,5);
	}
}

  printf("Terminei de ler\n");

  return 1;
}
/*
int readingFrame(int fd, unsigned char *frame) {
  unsigned char oneByte;
  int over = 0;
  int i = 0;

  // reading the first flag
  read(fd, &oneByte, 1);
  frame[i] = oneByte;
  i++;

  while (!over) {
    read(fd, &oneByte, 1);
    frame[i] = oneByte;

    // testing if is the last character
    if (oneByte == FLAG) {
      over = 1;
    }
    i++;
  }

  return i; // returning the size of the frame
}*/

int readingFrame(int fd, unsigned char *frame) {
  unsigned char oneByte;
  ReadingArrayState state = START;	
  int over = 0;
  int i = 0;

  while (!over) {
    read(fd, &oneByte, 1);
	
	switch(state){
		case START:
			if(oneByte == FLAG){
				frame[i] = oneByte;
				i++;
				state = FLAG_STATE;
			}
			break;
        case FLAG_STATE:
			if(oneByte != FLAG){
				frame[i] = oneByte;
        		i++;
				state = A_STATE;			
			}
			break;
		case A_STATE:
			if(oneByte != FLAG){
				frame[i] = oneByte;
        		i++;
				state = C_STATE;			
			}
			break;
        case C_STATE:
			if(oneByte != FLAG){
				frame[i] = oneByte;
        		i++;
				state = BCC;			
			}
			break;	
		case BCC:
			if(oneByte != FLAG){
				frame[i] = oneByte;
        		i++;			
			}
			else if(oneByte == FLAG){
				frame[i] = oneByte;
				i++;
				over = 1;
			}
			break;
		case SUCCESS:
		default:
			break;
	}
  }

  return i; // returning the size of the frame
}

int processingDataFrame(unsigned char *frame, FileInfo *file, int fp, int sizeAfterDestuffing) {
  int frameIndex = 4; // Where the packet starts
  int i;
  int numberOfBytes;
  int ret=1;
	int dataCounterCheck=0;

  if(frame[0] != FLAG){
	return -1;
  }

  if( frame[1] != A){
        return -1;
  }

  if( frame[2] != NUMBER_OF_SEQUENCE_0 && frame[2] != NUMBER_OF_SEQUENCE_1){
        return -1;
  }

  if(frame[3]!= (frame[1]^frame[2])){
      printf("BCC1 recebido: %X\n", frame[3]);
      printf("BCC1 esperado: %X\n",frame[1]^frame[2]);
      printf("ERRO BCC1\n");
    	return -1;
  }


  // Testing to see if is a control packet
  if (frame[frameIndex] == START_CTRL_PACKET ||
      frame[frameIndex] == END_CTRL_PACKET) {
    ret = frame[frameIndex];
    printf("Ret : %d\n", ret);
    frameIndex += 2; // TODO : Estou a ignorar o T

    numberOfBytes = frame[frameIndex];
    frameIndex++; // taking frame index to de begginning of the data
    memcpy(&((*file).size), frame + frameIndex, numberOfBytes);

    frameIndex += numberOfBytes + 1; // TODO : Estou a ignorar o T

    numberOfBytes = frame[frameIndex];
    frameIndex++;
    memcpy(&((*file).filename), frame + frameIndex, numberOfBytes);

    frameIndex += numberOfBytes;

    // TODO : processar o bcc2

    // FIXME: remove printf
    printf("Tamanho : %d\n", file->size);
    printf("Nome : %s\n", file->filename);
  } else if (frame[frameIndex] == DATA_CTRL_PACKET) {



    ret = frame[frameIndex];
	frameIndex++;
	int counterIndex=frameIndex;	
	frameIndex++;
   // frameIndex += 2; // TODO : Estou a ignorar o número de sequênon-canonical


    unsigned int l2 = frame[frameIndex];
    frameIndex++;
    unsigned int l1 = frame[frameIndex];
    frameIndex++;
    printf("l1 : %d\n" , l1);
    printf("l2 : %d\n" , l2);
    unsigned int k = 256 * l2 + l1;
	printf("k : %d\n", k);
	if(frame[8+k]!=getBCC2(frame+4,k+4)){
		printf("BCC RECEBIDO: %X\n",frame[8+k]);
		printf("BCC ESPERADO: %X\n",getBCC2(frame+4,k+4));
		return -1;
	}

	if(k!= (sizeAfterDestuffing-10)){
		printf("ERRO\n");
		return -1;
	}

	printf("Seq Number Receiver: %X \n",frame[counterIndex]);
	if(previousDataCounter==0){
		previousDataCounter=frame[counterIndex];
	}else{
		if(previousDataCounter==frame[counterIndex]){
			dataCounterCheck=1;
			printf("Repeated frame\n");
		}
		else{
			previousDataCounter=frame[counterIndex];
			dataCounterCheck=0;
		}
			
			
	}

    for (i = 0; i < k; i++) {
      // printf("%d : %X\n", i, frame[frameIndex+i]);
		if(dataCounterCheck==0)
      		write(fp, &frame[frameIndex + i], 1);
    }
  }



  return ret;
}

int stuffingFrame(unsigned char *frame, int frameSize) {
  int i;

  for (i = 1; i < frameSize - 1; i++) {
    if (frame[i] == FLAG) {
      frame[i] = ESC;
      i++;
      shiftFrame(frame, i, frameSize, 0);
      frameSize++;
      frame[i] = FLAG_HIDE_BYTE;
    }
    if (frame[i] == ESC) {
      i++;
      shiftFrame(frame, i, frameSize, 0);
      frameSize++;
      frame[i] = ESC_HIDE_BYTE;
    }
  }

  return frameSize;
}

int shiftFrame(unsigned char *frame, int i, int frameSize, int shiftDirection) {

  if (shiftDirection == 0) {

    frameSize--;
    for (; frameSize >= i; frameSize--) {
      frame[frameSize + 1] = frame[frameSize];
    }
  } else if (shiftDirection == 1) {
    int over = 0;
    i++;
    do {
      frame[i] = frame[i + 1];
      i++;
      if (frame[i] == FLAG) {
        over = 1;
      }
    } while (!over);
  }

  return 1;
}

int destuffingFrame(unsigned char *frame) {
  int over = 0;

  int i = 1;
  while (!over) {
    if (frame[i] == FLAG) {
      over = 1;
    } else if (frame[i] == ESC && frame[i + 1] == FLAG_HIDE_BYTE) {
      frame[i] = FLAG;
      shiftFrame(frame, i, 0, 1);
    } else if (frame[i] == ESC && frame[i + 1] == ESC_HIDE_BYTE) {
      shiftFrame(frame, i, 0, 1);
    }
    i++;
  }
  printf("tamanho apos destuffing: %d\n", i);
  return i;
}

unsigned char getBCC2(unsigned char * frame, unsigned int length){
	unsigned char BCC=0;

	unsigned int i=0;
	for(;i<length;i++){
		BCC ^= frame[i];
	}

	return BCC;
}
