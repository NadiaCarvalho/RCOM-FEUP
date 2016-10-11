/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "DataLinkLayer.h"

int flag=1, conta=1, success = 0, fail = 0;
int fd, c;
struct termios oldtio,newtio;

void atende()                   // atende alarme
{
  conta++;
  if(!success){
    if(conta<4){
      printf("alarme # %d\n", conta);
      alarm(3);
      flag=1;
      //send SET
      if(flag)
      printf("SENDER: sending SET\n");
      write(fd,SET,5);

    }
    if(conta==4){
      exit(1);
    }
  }
}

int llopen (char* SerialPort) {


 fd = open(SerialPort, O_RDWR | O_NOCTTY );
  if (fd <0) {perror(SerialPort); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) pr�ximo(s) caracter(es)
  */


  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");
	return fd;

}

int llclose(int fd){
	
  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr");
   return -1;
  }

  return close(fd);
	
}


int writenoncanonical(char* SerialPort)
{
  char buf[255];
  int i, sum = 0, speed = 0, res;


  if (  ((strcmp("/dev/ttyS0", SerialPort)!=0) &&
  (strcmp("/dev/ttyS1", SerialPort)!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd=llopen(SerialPort);
  //send SET
  res=write(fd,SET,5);
  printf("SENDER: sending SET: 0x%08X , 0x%08X , 0x%08X , 0x%08X , 0x%08X\n", SET[0], SET[1],SET[2],SET[3],SET[4]);
  (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

  strcpy(buf, "");
  if(flag){
    alarm(3);                 // activa alarme de 3s
    //receive UA
    char UA[5]="";
    fail=0;
	int i;
    for(i=0; i< 5; i++){
      if(!success){
        printf("SENDER: reading UA \n");
        res= read(fd,buf,1);
        printf("0x%08X \n", buf[0]);
        if( buf[0] != SET[i]){
          fail=1;
        }
      }


    }
    if(!fail){
      printf("SENDER: successful echoed UA");
      success=1;
      flag=0;
    }
  }

res = llclose(fd);


  return res;
}


int noncanonical(char* SerialPort)
{
  int c, res;
  char buf[255];

  if (  ((strcmp("/dev/ttyS0", SerialPort)!=0) &&
  (strcmp("/dev/ttyS1", SerialPort)!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

	fd=llopen(SerialPort);

  strcpy(buf, "");

  int i;
  printf("RECEIVER: reading SET\n" );
  for(i=0; i< 5; i++){
    res= read(fd,buf,1);
    printf("0x%08X \n", buf[0]);
    if( buf[0] != UA[i]){
      perror("Not equal");
    }
  }

  printf("RECEIVER: sending UA\n");
  write(fd, UA, 5);

  res=llclose(fd);
  
	return res;
}
