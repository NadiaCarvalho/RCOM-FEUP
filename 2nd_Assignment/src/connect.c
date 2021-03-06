#include "connection.h"

#define BUFFER_SIZE 512

int connect_to_server(int socket, struct addrinfo *res) {
  if (connect(socket, res->ai_addr, res->ai_addrlen) < 0) {
    perror("connect()");
    exit(0);
  }
  return 1;
}

int login_to_server(int sockfd, url *url_info) {

  char answer[MAXDATASIZE];
  char answer2[MAXDATASIZE];
  char user_password_command[MAXDATASIZE];
  char user_login_command[MAXDATASIZE];

  memset(user_login_command,0,sizeof(user_login_command));
  memset(user_password_command,0,sizeof(user_password_command));

  strcat(user_login_command, "USER ");
  strcat(user_login_command, url_info->user);
  strcat(user_login_command, "\r\n");
  printf("%s\n", user_login_command);


  write_to_server(sockfd, user_login_command);

  read_from_server(sockfd, answer);

  printf("%s\n",answer );

  strcat(user_password_command, "PASS ");
  strcat(user_password_command, url_info->password);
  strcat(user_password_command, "\r\n");
  printf("PASS: %s\n",user_password_command );


    write_to_server(sockfd, user_password_command);

    read_from_server(sockfd, answer2);

    printf("%s\n",answer2);


  return 1;
}

int set_PASV_mode(int sockfd, char answer[MAXDATASIZE]){
  char pasv_command[9]="PASV \r\n";

  write_to_server(sockfd, pasv_command);

  read_from_server(sockfd, answer);

  return 1;
}

int get_ip_adress(char answer3[MAXDATASIZE], char ip[100], char port1[100], char port2[100]){

  char *ret  = calloc(MAXDATASIZE, sizeof(char));;
  ret=strchr(answer3, '(');

  ret++;

  get_string(ret, 4, ip);
printf("11111111\n" );
  ret += strlen(ip);

  get_string(ret, 2, port1);

  ret += strlen(port1);

  get_string(ret, 2, port2);

  memmove(port1, port1+1, strlen(port1));
  memmove(port2, port2+1, strlen(port2));

  return 1;

}

int get_string(char *info, int numberOfCommas, char ret[100]){

  int i=0;
  int atualNumberOfCommas=0;
  while(1){
    if(*info == ',' || *info == ')'){
      atualNumberOfCommas++;
      if (atualNumberOfCommas == numberOfCommas){
        break;
      }
      else ret[i] = '.';
    }
    else {
      ret[i] = *info;
    }

    i++;
    info++;
  }

  ret[i] = '\0';


  return 1;
}

int write_to_server(int sockfd, const char *message) {
  int bytes = 0;

  bytes = write(sockfd, message, strlen(message));

  if (bytes == strlen(message))
    return 1;
  perror("Writting to server.");

  return 1;
}

int read_from_server(int sockfd, char answer[MAXDATASIZE]) {

  int numbytes = 0;

  if ((numbytes = recv(sockfd, answer, MAXDATASIZE - 1, 0)) == -1) {
    perror("Receiving from server");
    exit(1);
  }

  answer[numbytes] = '\0';

  return 1;
}

int asking_file_to_server(int socketfd, url *url_info){
  char file_path_to_download_command[MAXDATASIZE];
  memset(file_path_to_download_command,0,sizeof(file_path_to_download_command));
  char answer[MAXDATASIZE];

  strcat(file_path_to_download_command, "RETR ");
  strcat(file_path_to_download_command, url_info->url_path);
  strcat(file_path_to_download_command, "\r\n");

  if(write(socketfd, file_path_to_download_command, strlen(file_path_to_download_command)) < 0){
    perror("error on write retr command to the server");
    exit(-1);
  }

  read_from_server(socketfd, answer);

  printf("%s\n",answer);
  char ret[5];

  ret[0] = answer[0];
  ret[1] = answer[1];
  ret[2] = answer[2];
  ret[3] = '\0';

  int ret_int = atoi(ret);
  if(ret_int == 550){
    printf("File does not exit!Ending program!\n");
    exit(-1);
  }

  return 1;
}

int read_file_from_server(int datafd,char filename[MAXDATASIZE]){
  int new_file_fd = open(filename, O_CREAT|O_WRONLY, 0777);
  int ret;
  char buffer[MAXDATASIZE];

  if(new_file_fd < 0){
    perror("Error open file");
    exit(-1);
  }

  printf("Start reading file\n\n");


  while((ret=read(datafd,buffer, BUFFER_SIZE)) > 0){
    if(ret == -1){
      perror("Error reading file from server");
      exit(-1);
    }

    if(write(new_file_fd,buffer,ret) < 0){
      perror("Error writing to the destination file");
      exit(-1);
    }
  }

  printf("File read, ending program\n");

  return 1;
}
