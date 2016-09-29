/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) ||    
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

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
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

	unsigned char UA[5];
	UA[0]=FLAG;
	UA[1]=A;
	UA[2]=C_SET;
	UA[3]=UA[1]^UA[2];
	UA[4]=FLAG;

	strcpy(buf, "");
	read(fd,buf,5*sizeof(char);

	res = strcmp(UA, buf);
	if(res != 0){
		perror("Not equal");
		exit(-1);
	} 

	write(fd, UA, 5*sizeof(char));

	char result[255];
	strcpy(buf, "");

    	while (STOP==FALSE) {       /* loop for input */
      		res = read(fd,buf,1);   /* returns after 1 chars have been input */
      		buf[res]=0;  
      		strcat(result,buf);
            
      		if (buf[0]=='\0') STOP=TRUE;
    	}

	printf("Received: %s \n",result);
	
    	res = write(fd,result,strlen(result)+1);

  	printf("%d bytes written\n", res);


    	tcsetattr(fd,TCSANOW,&oldtio);
    	close(fd);
    	return 0;
}
