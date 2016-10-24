#include "Utilities.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int getFile(char *filepath) {

  printf("Enter file path:");
  scanf("%s", filepath);

  return 1;
}

int fileSize(FILE *fd) {
  struct stat s;
  if (fstat(fileno(fd), &s) == -1) {
    return (-1);
  }
  return (s.st_size);
}
