#include "connection.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once

int connect_to_server(int socket, struct addrinfo *res) {
  if (connect(socket, res->ai_addr, res->ai_addrlen) < 0) {
    perror("connect()");
    exit(0);
  }
  return 1;
}

int login_to_server(int sockfd) {

  char user[255];
  char *answer = malloc(100 * sizeof(char));
  struct termios term, oldterm;
  char pass[255], ch, echo = '*';

  printf("Username: \n");
  scanf("%s", user);

  write_to_server(sockfd, user);

  read_from_server(sockfd, answer);

  if (strcmp(answer, "331") != 0) {
    perror("Error sending username.");
  }

  free(answer);

  printf("Username: \n");

  tcgetattr(STDIN_FILENO, &oldterm);
  term = oldterm;
  term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);

  int i = 0;
  while (i < MAXDATASIZE && read(STDIN_FILENO, &ch, 1) && ch != '\n') {
    pass[i++] = ch;
    write(STDOUT_FILENO, &echo, 1);
  }
  pass[i] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);

  write_to_server(sockfd, pass);
  read_from_server(sockfd, answer);

  if (strcmp(answer, "230") != 0) {
    perror("Error login in: Wrong password.");
  }

  return 1;
}

int write_to_server(int sockfd, const char *message) {
  /*send a string to the server*/
  int bytes = 0;
  bytes = write(sockfd, message, strlen(message));

  if (bytes == strlen(message))
    return 1;
  perror("Writting to server.");
}

int read_from_server(int sockfd, char *answer) {
  int numbytes = 0;
  if ((numbytes = recv(sockfd, answer, MAXDATASIZE - 1, 0)) == -1) {
    perror("Receiving from server");
    exit(1);
  }
  answer[numbytes] = '\0';
  return 1;
}
