/*      (C)2000 FEUP  */
#include "utilities.h"
#include "connection.h"

// login anonymous
// password anything

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


int main(int argc, char **argv) {

	char answer[MAXDATASIZE];
	char answer3[MAXDATASIZE];

  if (argc != 2) {
    printf("\n\nInvalid arguments, expected:\n");
    printf(" ./app ftp://[<user>:<password>@]<host>/<url-path>\n\n");
		return -1;
  }

	url *url_info = malloc(sizeof(url));
	getInfo(argv[1], url_info);

	int ret = strcmp(url_info->user,"anonymous");

	if(ret != 0){
		printf("Error! User must be anonymous\n");
		exit(-1);
	}

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

	printf("%s\n\n", "Logged in");

	set_PASV_mode(sockfd, answer3);
	printf("%s\n", "Passive mode set !");

	char ip[100];
	char port1[100];
	char port2[100];

	get_ip_adress(answer3, ip, port1, port2);

	printf("Ip : %s\n", ip);

	int port = atoi(port1) * 256 + atoi(port2);

	printf("Port : %d\n\n", port);

	int datafd = initTCP(ip, port);

	asking_file_to_server(sockfd,url_info);

	char filename[MAXDATASIZE];

	get_filename(url_info->url_path, filename);

	printf("filename: %s\n", filename);

	read_file_from_server(datafd,filename);

	close(datafd);
  close(sockfd);
  exit(0);
}
