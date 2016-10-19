#include "Utilities.h"

char getFile(){
  char filepath[255];

  printf("Enter file path:" );
  scanf("%s", filepath);

  return filepath;
}
