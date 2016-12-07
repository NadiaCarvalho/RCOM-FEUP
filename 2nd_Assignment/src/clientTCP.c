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


	if(0){
    printf("%s\n",url_info->type);
    printf("%s\n",url_info->user );
    printf("%s\n",url_info->password);
    printf("%s\n",url_info->host);
    printf("%s\n",url_info->url_path);
  }

  int sockfd;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // TODO: verificar se é efetivamente a porta 21 !
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
/*	memp(answer2, answer, 3);
	printf("%s\n", answer2);
	if(strcmp(answer2,"220")){
		printf("%s\n", "Connection established");
	}
*/
	//printf("Connection established:%s\n", answer);

  login_to_server(sockfd, url_info);
	printf("%s\n", "logged in");

	set_PASV_mode(sockfd, answer3);
	printf("%s\n", "Passive mode set !");

	char ip[100];
	char port1[100];
	char port2[100];

	get_ip_adress(answer3, ip,port1,port2);


  close(sockfd);
  exit(0);
}
