/*      (C)2000 FEUP  */
#include "connection.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

int main(int argc, char** argv){

	int	sockfd;
	struct  addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//TODO: verificar se é efetivamente a porta 21 !
	if(getaddrinfo(argv[1],"21",&hints,&res)!=0){
		fprintf(stderr, "getaddrinfo: %s\n", "ERROR");
		exit(0);
	}
/*
	//server address handling
	bzero((char*)&server_addr,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	//32 bit Internet address network byte ordered
	server_addr.sin_port = htons(SERVER_PORT);		//server TCP port must be network byte ordered
*/

	/*open an TCP socket*/
	if ((sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) < 0) {
    		perror("socket()");
        	exit(0);
    	}


	/*connect to the server*/
	connect_to_server(sockfd,res);

	login_to_server(sockfd);




	close(sockfd);
	exit(0);
}
