#include "utilities.h"

int getInfo(char * str,url * url_info){

  char * token = malloc(strlen(str));

  url_info->type = malloc(strlen(token));
  url_info->user = malloc(strlen(token));
  url_info->password = malloc(strlen(token));
  url_info->host = malloc(strlen(token));
  url_info->url_path = malloc(strlen(token));

  memcpy(token, str, strlen(str));
  memcpy(url_info->type, token, 6);
  token = strtok(token+6, ":");
  memcpy(url_info->user, token, strlen(token));
  token = strtok(NULL, "@");
  memcpy(url_info->password, token, strlen(token));
  token = strtok(NULL, "/");
  memcpy(url_info->host, token, strlen(token));
  token = strtok(NULL, "");
  memcpy(url_info->url_path, token, strlen(token));

  if(0){
    printf("%s\n",url_info->type);
    printf("%s\n",url_info->user );
    printf("%s\n",url_info->password);
    printf("%s\n",url_info->host);
    printf("%s\n",url_info->url_path);
  }
  return 1;
}

int get_filename(char path[100], char * filename){

  char * token = malloc(strlen(path));
  memcpy(token, path, strlen(path));
  token = strtok(path, "/");
  while(token != NULL){
    memcpy(filename, token, strlen(token));
    token = strtok(NULL, "/");
  }

  printf("Starting reading file : %s\n", filename);
  return 1;
}
