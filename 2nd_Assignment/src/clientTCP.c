/*      (C)2000 FEUP  */
#include "utilities.h"
#include "connection.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

// login anonymous
// password anything

int main(int argc, char **argv) {

	char *answer = malloc(100 * sizeof(char));
	char *answer2 = malloc(3 * sizeof(char));
	char *answer3 = malloc(100 * sizeof(char));
  if (argc != 2) {
    printf("\n\nInvalid arguments, expected:\n");
    printf(" ./app ftp://[<user>:<password>@]<host>/<url-path>\n\n");
		return -1;
  }

	url *url_info = malloc(sizeof(url));
	getInfo(argv[1], url_info);


  int sockfd;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
 
  if (getaddrinfo(url_info->host, "21", &hints, &res) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", "ERROR");
    exit(0);
  }

  /*open an TCP socket*/
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <
      0) {
    perror("socket()");
    exit(0);
  }

  connect_to_server(sockfd, res);
  read_from_server(sockfd, answer);

 
  login_to_server(sockfd, url_info);

	printf("%s\n", "logged in");

	set_PASV_mode(sockfd, answer3);
	printf("%s\n", "Passive mode set !");

	char ip[100];
	char port1[100];
	char port2[100];

	get_ip_adress(answer3, ip, port1, port2);

	printf("Ip : %s\n", ip);

	int port = atoi(port1) * 256 + atoi(port2);

	printf("Port : %d\n", port);

	int datafd = initTCP(ip, port);

	printf("BBBBBBB: %s\n",url_info->url_path); 

	asking_file_to_server(sockfd,url_info);

	char * filename = malloc(strlen(url_info->url_path));

	get_filename(url_info->url_path, filename);

	read_file_from_server(datafd,filename);

	close(datafd);
  close(sockfd);
  exit(0);
}

int initTCP(char *address, int port) {
    int	sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(address);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		        /*server TCP port must be network byte ordered */

    /*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    	perror("socket()");
      exit(-1);
    }

	/*connect to the server*/
    if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
      perror("connect()");
			exit(-1);
		}

    return sockfd;
}
