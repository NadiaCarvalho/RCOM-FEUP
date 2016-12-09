#pragma once
#include "utilities.h"

int connect_to_server(int socket, struct  addrinfo *res);

int login_to_server(int sockfd, url *url_info);

int set_PASV_mode(int sockfd, char *answer);

int get_ip_adress(char *answer3, char ip[100], char port1[100], char port2[100]);

int get_string(char *info, int numberOfCommas, char ret[100]);

int write_to_server(int sockfd, const char *message);

int read_from_server(int sockfd, char * answer);

int asking_file_to_server(int socketfd, url *url_info);

int read_file_from_server(int datafd,char *filename);
